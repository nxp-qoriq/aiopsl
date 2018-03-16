/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_types.h"
#include "fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_parser.h"
#include "fsl_l2.h"
#include "fsl_table.h"
#include "fsl_keygen.h"
#include "fsl_osm.h"
#include "fsl_dbg.h"
#include "fsl_evmng.h"
#include "fsl_stdlib.h"
#include "fsl_gen.h"
#include "classifier.h"

#define AIOP_APP_NAME		"CLASSIFIER"

/*
 * The application swaps source & destination addresses for L2 and IPv4
 * protocols, so it needs only 64 bytes from the frame.
 * The Segment Presentation Size (SPS) is set to 64
 * as smaller SPS can improve performance.
 */
#define PRESENTATION_LENGTH 64

/* IPV4: SrcIP(4), DstIP(4), Protocol(1), SrcPort(2), DstPort(2) */
#define TABLE_KEY_LEN		13
#define TABLE_COMMIT_RULES	10
#define TABLE_MAX_RULES		20
#define CONN_TABLE_SIZE		3

#define ORDER_MODE_NONE		0	/* Order mode None */
#define ORDER_MODE_EXCLUSIVE	1	/* Order mode Exclusive */
#define ORDER_MODE_CONCURRENT	2	/* Order mode Concurrent */

/* IP (generic) fields */
#define NH_FLD_IP_VER           (1)
#define NH_FLD_IP_PROTO         (NH_FLD_IP_VER << 4)
#define NH_FLD_IP_SRC           (NH_FLD_IP_VER << 5)
#define NH_FLD_IP_DST           (NH_FLD_IP_VER << 6)
/* L4 fields */
#define NH_FLD_L4_PORT_SRC	(1)
#define NH_FLD_L4_PORT_DST	(NH_FLD_L4_PORT_SRC << 1)

struct classif_connection {
	uint8_t		key[TABLE_KEY_LEN];
	uint8_t		order_mode;
};

static struct classif_connection conn[CONN_TABLE_SIZE] = {
	 /* IP_SRC | IP_DST | PROTO | PORT_SRC | PORT_DST */
/* TCP */{{ 0xC6, 0x14, 0x01, 0x01,
	    0xC6, 0x13, 0x01, 0x00,
	    0x06,
	    0x04, 0x00,
	    0x04, 0x01},
	    ORDER_MODE_EXCLUSIVE},
/* UDP */{{ 0xC6, 0x14, 0x01, 0x01,
	    0xC6, 0x13, 0x01, 0x40,
	    0x11,
	    0x04, 0x00,
	    0x04, 0x01},
	    ORDER_MODE_CONCURRENT},
/* SCTP*/{{ 0xC6, 0x14, 0x01, 0x01,
	    0xC6, 0x13, 0x01, 0x80,
	    0x84,
	    0x04, 0x00,
	    0x04, 0x01},
	    ORDER_MODE_NONE}
};

static uint8_t key_id = 0xff;
static uint16_t table_id = 0xffff;

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg);
static int app_create_exact_match_table(void);
static inline void l2_ip_src_dst_swap(void);
#ifndef LS2085A_REV1
static int app_query_table_rules(void);
#endif

#ifdef PRINT_FRAME_INFO
/* Prints DPNI on which frame was received, together with CORE and IP & MAC
 * source and destination addresses */
static uint64_t print_frame_mutex;
static void print_frame_info(void);
#else
	#define print_frame_info()
#endif

/* Performs frames processing */
__HOT_CODE ENTRY_POINT static void app_classifier(void)
{
	struct table_lookup_result  lk_result __attribute__((aligned(16)));
	int			err = 0;
	uint32_t	flags = DPNI_DRV_SEND_MODE_TERM;

	sl_prolog();

	if (!PARSER_IS_OUTER_IPV4_DEFAULT()) {
		/* Discard non IPV4 frame and terminate task */
		goto drop_frame;
	}

	/* Perform a lookup with created key */
	if (table_lookup_by_keyid_default_frame(TABLE_ACCEL_ID_CTLU, table_id,
		key_id, &lk_result) != TABLE_STATUS_SUCCESS)
		goto drop_frame;

	if (GET_USER_DATA(lk_result) == ORDER_MODE_NONE) {
		/* Transition to mode NONE */
		osm_scope_exit();
	} else if (GET_USER_DATA(lk_result) == ORDER_MODE_EXCLUSIVE) {
		/* Transition to mode EXCLUSIVE */
		osm_scope_transition_to_exclusive_with_increment_scope_id();
		ADD_DPNI_DRV_SEND_RELINQUISH_MODE(flags);
	}

	/* Print frame to be processed */
	print_frame_info();

	/* Swap source & destination addresses for L2 and IP protocols */
	l2_ip_src_dst_swap();

	/* Modify the data in the default Data Segment */
	fdma_modify_default_segment_full_data();

	if (GET_USER_DATA(lk_result) == ORDER_MODE_CONCURRENT) {
		/* Transition to mode EXCLUSIVE */
		osm_scope_transition_to_exclusive_with_increment_scope_id();
		ADD_DPNI_DRV_SEND_RELINQUISH_MODE(flags);
	}

	err = dpni_drv_send(task_get_receive_niid(), flags);
	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();

	pr_err("Failed to send frame\n");
	fdma_terminate_task();

drop_frame:
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

static int app_dpni_link_up_cb(uint8_t generator_id, uint8_t event_id,
				uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : ni%d link is UP\n", AIOP_APP_NAME, ni);
	return 0;
}

static int app_dpni_link_down_cb(uint8_t generator_id, uint8_t event_id,
				  uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : ni%d link is DOWN\n", AIOP_APP_NAME, ni);
	return 0;
}

static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	struct ep_init_presentation init_presentation;

	UNUSED(generator_id);
	UNUSED(event_id);

	/* Get endpoint and skip DPNI-DPNI connection from app initialization */
	err = dpni_drv_get_connected_obj(ni, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		pr_err("Cannot get connected object for NI %d\n", ni);
		return err;
	}
	if (strcmp(dpni_ep_type, "dpni") == 0)
		return 0;

	/* Get DPNI ID for current Network Interface ID*/
	err = dpni_drv_get_dpni_id(ni, &dpni_id);
	if (err) {
		pr_err("Cannot get DPNI ID for NI %d\n", ni);
		return err;
	}

	/* Configure frame processing callback */
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure processing callback on NI %d\n", ni);
		return err;
	}

	/* Enable promiscuous mode */
	err = dpni_drv_set_unicast_promisc(ni, 1);
	if (err) {
		pr_err("Cannot enable promiscuous mode on NI %d\n", ni);
		return err;
	}

	/* Default ordering mode should be Concurrent */
	err = dpni_drv_set_concurrent(ni);
	if (err) {
		pr_err("Cannot set Concurrent Execution on NI %d\n", ni);
		return err;
	}

	/* Configure key generation for initial ordering scope */
	app_fill_kg_profile(&kg_cfg);
	err = dpni_drv_set_order_scope(ni, &kg_cfg);
	if (err) {
		pr_err("Cannot set order scope on NI %d\n", ni);
		return err;
	}

	/* Get DPNI MAC address  */
	err = dpni_drv_get_primary_mac_addr(ni, mac_addr);
	if (err) {
		pr_err("Cannot obtain primary MAC ADDR for NI %d\n", ni);
		return err;
	}

	/* Set the initial segment presentation size */
	err = dpni_drv_get_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot get initial presentation for NI %d\n", ni);
		return err;
	}

	init_presentation.options = EP_INIT_PRESENTATION_OPT_SPS;
	init_presentation.sps = PRESENTATION_LENGTH;
	err = dpni_drv_set_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot set initial presentation for NI %d to %d\n",
				ni, init_presentation.sps);
		return err;
	}

	/* Enable DPNI to receive frames */
	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("Cannot enable NI %d for Rx/Tx\n", ni);
		return err;
	}

	fsl_print("%s : Successfully configured ni%d (dpni.%d)\n",
		  AIOP_APP_NAME, ni, dpni_id);
	fsl_print("%s : dpni.%d <---connected---> %s.%d ",
		  AIOP_APP_NAME, dpni_id, dpni_ep_type, dpni_ep_id);
	fsl_print("(MAC addr: %02x:%02x:%02x:%02x:%02x:%02x)\n",
		  mac_addr[0], mac_addr[1], mac_addr[2],
		  mac_addr[3], mac_addr[4], mac_addr[5]);

	return 0;
}

/* Early initialization */
int app_early_init(void)
{
	return 0;
}

/* Initializes the application */
int app_init(void)
{
	uint16_t	ni;
	int		err;

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t) app_classifier, app_dpni_add_cb);
	if (err) {
		pr_err("Cannot register DPNI ADD event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_UP, 1,
			     (uint64_t) NULL, app_dpni_link_up_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK UP event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_DOWN, 1,
			     (uint64_t) NULL, app_dpni_link_down_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK DOWN event\n");
		return err;
	}

	err = app_create_exact_match_table();
	if (err) {
		pr_err("%s : Cannot initialize application\n!",
			AIOP_APP_NAME);
		goto unregister_cbs;
	}
	fsl_print("%s : Successfully configured exact table match\n",
		AIOP_APP_NAME);
#ifndef LS2085A_REV1
	err = app_query_table_rules();
	if (err) {
		pr_err("%s : Cannot initialize application\n!", AIOP_APP_NAME);
		goto unregister_cbs;
	}
#endif
	return 0;

unregister_cbs:
	for (ni = 0; ni < dpni_drv_get_num_of_nis(); ni++)
		dpni_drv_unregister_rx_cb(ni);
	return err;

}

/* Frees application allocated resources */
void app_free(void)
{}

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg)
{
	/* Configure Initial Order Scope */
	memset(kg_cfg, 0x0, sizeof(struct dpkg_profile_cfg));

	kg_cfg->num_extracts = 5;
	/* IP_SRC */
	kg_cfg->extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[0].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[0].extract.from_hdr.field = NH_FLD_IP_SRC;
	/* IP_DST: mask last byte */
	kg_cfg->extracts[1].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[1].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[1].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[1].extract.from_hdr.field = NH_FLD_IP_DST;
	kg_cfg->extracts[1].num_of_byte_masks = 1;
	kg_cfg->extracts[1].masks[0].offset = 3;
	/* PROTO */
	kg_cfg->extracts[2].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[2].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[2].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[2].extract.from_hdr.field = NH_FLD_IP_PROTO;
	/* L4 SRC PORT */
	kg_cfg->extracts[3].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[3].extract.from_hdr.prot = NET_PROT_TCP;
	kg_cfg->extracts[3].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[3].extract.from_hdr.field = NH_FLD_L4_PORT_SRC;
	/* L4 DST PORT */
	kg_cfg->extracts[4].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[4].extract.from_hdr.prot = NET_PROT_TCP;
	kg_cfg->extracts[4].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[4].extract.from_hdr.field = NH_FLD_L4_PORT_DST;
}

static int app_create_exact_match_table(void)
{
	struct kcr_builder	kb __attribute__((aligned(16)));
	struct table_rule	rule __attribute__((aligned(16)));
	struct kcr_builder_fec_mask	mask;
	struct table_create_params	tbl_params;
	uint16_t			l_table_id;
	uint8_t				l_key_id;
	uint64_t			tmp_rule_id = 0;
	int				ret;

	/* Create key composition rule */
	memset(&kb, 0, sizeof(struct kcr_builder));
	keygen_kcr_builder_init(&kb);
	/* IP_SRC */
	ret = keygen_kcr_builder_add_protocol_specific_field(
			KEYGEN_KCR_IPSRC_1_FECID, NULL, &kb);
	if (ret) {
		pr_err("Failed to add IP_SRC to key rule: ret = %d\n", ret);
		return ret;
	}
	/* IP_DST:  mask least significant 5 bits of IP DST */
	memset(&mask, 0, sizeof(struct kcr_builder_fec_mask));
	mask.num_of_masks = 1;
	mask.single_mask[0].mask = 0xE0;
	mask.single_mask[0].mask_offset = 3;
	ret = keygen_kcr_builder_add_protocol_specific_field(
			KEYGEN_KCR_IPDST_1_FECID, &mask, &kb);
	if (ret) {
		pr_err("Failed to add IP_DST to key rule: ret = %d\n", ret);
		return ret;
	}
	/* PTYPE */
	ret = keygen_kcr_builder_add_protocol_specific_field(
			KEYGEN_KCR_PTYPE_1_FECID, NULL, &kb);
	if (ret) {
		pr_err("Failed to add PTYPE to key rule: ret = %d\n", ret);
		return ret;
	}
	/* SRC_PORT */
	ret = keygen_kcr_builder_add_protocol_specific_field(
			KEYGEN_KCR_L4PSRC_FECID, NULL, &kb);
	if (ret) {
		pr_err("Failed to add SRC_PORT to key rule: ret = %d\n", ret);
		return ret;
	}
	/* DST_PORT */
	ret = keygen_kcr_builder_add_protocol_specific_field(
			KEYGEN_KCR_L4PDST_FECID, NULL, &kb);
	if (ret) {
		pr_err("Failed to add DST_PORT to key rule: ret = %d\n", ret);
		return ret;
	}
	/* Generate key */
	ret = keygen_kcr_create(
			KEYGEN_ACCEL_ID_CTLU, (uint8_t *)&kb.kcr, &l_key_id);
	if (ret) {
		pr_err("Failed to create exact match key composition rule : ret = %d\n",
			ret);
		return ret;
	}

	/* Create exact match table */
	memset(&tbl_params, 0, sizeof(struct table_create_params));
	
	tbl_params.attributes = APP_TABLE_PARAMS_ATTRIBUTES;
	tbl_params.key_size = TABLE_KEY_LEN;
	tbl_params.committed_rules = TABLE_COMMIT_RULES;
	tbl_params.max_rules = TABLE_MAX_RULES;
	ret = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params, &l_table_id);
	if (ret) {
		keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU, l_key_id);
		pr_err("Failed to create exact match table : ret = %d\n", ret);
		return ret;
	}
	/* Create rules for exact match table */
	for (uint8_t i = 0; i < CONN_TABLE_SIZE; i++) {
		memset(&rule, 0, sizeof(struct table_rule));
		memcpy(&rule.key_desc.em.key,
				(const void *)&conn[i].key, TABLE_KEY_LEN);

		rule.result.type = ARCH_TABLE_RESULT_TYPE;
		SET_USER_DATA(rule, conn[i].order_mode);
		rule.options = TABLE_RULE_TIMESTAMP_NONE;

		ARCH_TABLE_RULE_CREATE(l_table_id, rule, TABLE_KEY_LEN, 
					tmp_rule_id);

		if (ret != TABLE_STATUS_SUCCESS) {
			pr_err("Failed to add rule to table(table_id=%d)\n",
				l_table_id);
			goto rule_create_err;
		}
	}

	/* Save key composition rule ID and classification table ID */
	key_id = l_key_id;
	table_id = l_table_id;

	return 0;

rule_create_err:
	keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU, l_key_id);
	table_delete(TABLE_ACCEL_ID_CTLU, l_table_id);

	return ret;
}

static inline void l2_ip_src_dst_swap(void)
{
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	uint8_t  *ethhdr = PARSER_GET_ETH_POINTER_DEFAULT();
	uint8_t  dst_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	struct   ipv4hdr *ipv4_hdr;
	uint32_t ip_src_addr;
	uint8_t	 *eth_src, *eth_dst;

	/* get ETH source and destination addresses */
	eth_dst = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT());
	eth_src = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT() +
					NET_HDR_FLD_ETH_ADDR_SIZE);

	/* store MAC_DST */
	*((uint32_t *)&dst_addr[0]) = *((uint32_t *)eth_dst);
	*((uint16_t *)&dst_addr[4]) = *((uint16_t *)(eth_dst + 4));

	/* set ETH destination address */
	*((uint32_t *)(&ethhdr[0])) = *((uint32_t *)(eth_src));
	*((uint16_t *)(&ethhdr[4])) = *((uint16_t *)(eth_src + 4));

	/* set ETH source address */
	*((uint32_t *)(&ethhdr[6])) = *((uint32_t *)&dst_addr[0]);
	*((uint16_t *)(&ethhdr[10])) = *((uint16_t *)&dst_addr[4]);

	/* get IPv4 header */
	ipv4_hdr = (struct ipv4hdr *)((uint8_t *)
			PRC_GET_SEGMENT_ADDRESS() +
			(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	/* store IP source address before changing it */
	ip_src_addr = ipv4_hdr->src_addr;
	/* swap IP source & destination addresses */
	ipv4_hdr->src_addr = ipv4_hdr->dst_addr;
	ipv4_hdr->dst_addr = ip_src_addr;

	/* we do not need to update nor the IP, nor the L4 checksum, because
	 * the IP source & destination addresses were swapped and not replaced
	 * with other values */

	/* Mark running sum as invalid */
	pr->gross_running_sum = 0;
}

#ifndef LS2085A_REV1
static int app_query_table_rules(void)
{
	t_rule_id rule_id = 0, next_rule_id;
	union table_key_desc key_desc;
	int i, rule_idx = 1, status;
	uint32_t rule_id_h, rule_id_l;

	fsl_print("Rule #    IP Src.     IP Dst.     Proto SPort   DPort\n");

	do {
		status = table_get_next_ruleid(TABLE_ACCEL_ID_CTLU,
					       table_id,
					       rule_id,
					       &next_rule_id);
		if (status)
			break;
		rule_id_h = (uint32_t)((next_rule_id >> 32) & 0xFFFFFFFF);
		rule_id_l = (uint32_t)(next_rule_id & 0xFFFFFFFF);

		status = table_rule_query_get_key_desc(TABLE_ACCEL_ID_CTLU,
						       table_id, next_rule_id,
						       &key_desc);
		if (status) {
			pr_err("Failed to query rule_id_h = 0x%08x, rule_id_l = 0x%08x\n",
			       rule_id_h, rule_id_l);
			return status;
		}

		fsl_print("%02d        0x", rule_idx);

		for (i = 0; i < TABLE_KEY_LEN; i++) {
			if (i == 4 || i == 8 || i == 9 || i == 11)
				fsl_print("  0x");

			fsl_print("%02x", key_desc.em.key[i]);
		}

		fsl_print("\n");

		rule_idx++;
		rule_id = next_rule_id + 1;
	} while (1);

	return 0;
}
#endif
#ifdef PRINT_FRAME_INFO
static void print_frame_info(void)
{
	struct ipv4hdr  *ipv4_hdr;
	uint8_t		*ip_src, *ip_dst, *eth_src, *eth_dst;

	/* Get IPv4 header */
	ipv4_hdr = (struct ipv4hdr *)((uint8_t *)
			PRC_GET_SEGMENT_ADDRESS() +
			(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	/* Lock mutex */
	cdma_mutex_lock_take(print_frame_mutex, CDMA_MUTEX_WRITE_LOCK);

	fsl_print("\nRX on NI %d | CORE:%d\n",
		(uint16_t)PRC_GET_PARAMETER(), core_get_id());

	/* Get Ethernet Source & Destination addresses */
	eth_dst = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT());
	eth_src = eth_dst + NET_HDR_FLD_ETH_ADDR_SIZE;

	fsl_print("  MAC_SA: %02x-%02x-%02x-%02x-%02x-%02x",
		eth_src[0], eth_src[1], eth_src[2],
		eth_src[3], eth_src[4], eth_src[5]);

	fsl_print(" MAC_DA: %02x-%02x-%02x-%02x-%02x-%02x\n",
		eth_dst[0], eth_dst[1], eth_dst[2],
		eth_dst[3], eth_dst[4], eth_dst[5]);

	/* Get IP Source & Destination addresses */
	ip_src = (void *)&ipv4_hdr->src_addr;
	ip_dst = (void *)&ipv4_hdr->dst_addr;
	fsl_print("  IP_SRC: %d.%d.%d.%d IP_DST: %d.%d.%d.%d\n",
		ip_src[0], ip_src[1], ip_src[2], ip_src[3],
		ip_dst[0], ip_dst[1], ip_dst[2], ip_dst[3]);

	/* Unlock mutex */
	cdma_mutex_lock_release(print_frame_mutex);
}
#endif
