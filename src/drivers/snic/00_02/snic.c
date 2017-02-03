/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		snic.c

@Description	This file contains the AIOP snic source code.

*//***************************************************************************/

#ifdef ENABLE_SNIC

#include "snic.h"
#include "system.h"
#include "fsl_net.h"
#include "fsl_stdio.h"
#include "fsl_string.h"
#include "fsl_errors.h"
#include "fsl_platform.h"
#include "fsl_io.h"
#include "fsl_aiop_common.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"

#include "fsl_parser.h"
#include "fsl_l2.h"
#include "fsl_fdma.h"
#include "fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_snic_cmd.h"

#include "general.h"
#include "osm_inline.h"

#include "fsl_ipf.h"
#include "fsl_table.h"
#include "fsl_cmdif_server.h"

#include "fsl_malloc.h"
#include "fsl_sl_slab.h"

#include "fsl_gso.h"
#include "fsl_gro.h"


#define SNIC_CMD_READ(_param, _offset, _width, _type, _arg) \
	_arg = (_type)mc_dec(cmd_data->params[_param], _offset, _width);

#define SNIC_CMD_READ_BYTE_ARRAY(_param, _offset, _width, _type, _arg) \
{\
		int i, u_param = _param, u_offset = _offset, _array_size = _width/8;\
	 	 for (i = 0; i < _array_size; i++) {\
	 		 u_offset = (_offset + (i * 8)) % 64;\
	 		 if (u_offset == 0)\
	 		 	 u_param += 1;\
	 		 	 ((*(_arg + i)) |= (_type)mc_dec(cmd_data->params[u_param], (u_offset), (8)));\
	 	 }\
	}

#define SNIC_RSP_PREP(_param, _offset, _width, _type, _arg) \
	cmd_data->params[_param] |= mc_enc(_offset, _width, _arg);

/** This is where FQD CTX should reside */
#define FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx)
/** Get sNIC ID from dequeue context */
#define SNIC_ID_GET \
	(uint16_t)(LLLDW_SWAP((uint32_t)&FQD_CTX_GET, 0) & 0xFFFF)
/** Get sNIC modes from dequeue context */
#define SNIC_IS_INGRESS_GET \
	(uint32_t)(LLLDW_SWAP((uint32_t)&FQD_CTX_GET, 0) & 0x80000000)

extern __TASK struct aiop_default_task_params default_task_params;

struct snic_params snic_params[MAX_SNIC_NO];
uint8_t snic_tmi_id;
uint64_t snic_tmi_mem_base_addr;

struct tcp_gro_context_params snic_tcp_gro_param[MAX_SNIC_NO];
uint64_t snic_gro_stats_addr;
uint64_t snic_gro_addr;

__HOT_CODE static inline void snic_set_enqueue_param(uint16_t snic_id,
			struct fdma_queueing_destination_params *enqueue_params)
{
	/* for the enqueue set hash from TLS, an flags equal 0 meaning that
	 * the qd_priority is taken from the TLS and that enqueue function
	 * always returns */
	enqueue_params->qdbin = 0;
	enqueue_params->qd = snic_params[snic_id].qdid;
	enqueue_params->qd_priority = default_task_params.qd_priority;
}

__HOT_CODE static inline int snic_send(
		struct fdma_queueing_destination_params *enqueue_params,
		uint32_t flags)
{
	int err = fdma_store_and_enqueue_default_frame_qd(enqueue_params,
							  FDMA_ENWF_NO_FLAGS);
	if (err) {
		if (err == -ENOMEM)
			fdma_discard_default_frame(flags);
		else /* (err == -EBUSY) */
			fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS, 0,
					(FDMA_DIS_AS_BIT | flags));
	}
	return err;

}

__HOT_CODE static void snic_tcp_gro_timeout_cb(uint64_t snic_id)
{
	struct fdma_queueing_destination_params enqueue_params;

	*((uint8_t *)HWC_SPID_ADDRESS) = snic_params[snic_id].spid;

	/* snic uses only 1 QDID so we need to have different
	 * qd/priority for ingress than for egress */
	default_task_params.qd_priority = 8;

	snic_set_enqueue_param((uint16_t)snic_id, &enqueue_params);

	snic_send(&enqueue_params, FDMA_DIS_FRAME_TC_BIT);
	fdma_terminate_task();
}

static void snic_set_tcp_gro_param(int id, uint64_t stats_addr, uint8_t tmi_id)
{
	struct tcp_gro_context_params *param = &snic_tcp_gro_param[id];

	param->timeout_params.gro_timeout_cb_arg = 0;	/* snic_id */
	param->timeout_params.gro_timeout_cb = snic_tcp_gro_timeout_cb;
	param->timeout_params.granularity =
			TCP_GRO_CREATE_TIMER_MODE_10_MSEC_GRANULARITY;
	param->timeout_params.tmi_id = tmi_id;

	param->limits.timeout_limit = 0xffe0;
	param->limits.packet_size_limit = 0xffff;
	param->limits.seg_num_limit = 128;

	param->metadata_addr = 0;
	param->stats_addr = stats_addr;
}

static void snic_reset_tcp_gro_ctx(uint16_t id)
{
	tcp_gro_ctx_t tmp;

	snic_params[id].tcp_gro_ctx = snic_gro_addr + id * TCP_GRO_CONTEXT_SIZE;
	snic_tcp_gro_param[id].timeout_params.gro_timeout_cb_arg = id;

	/* clear GRO context */
	cdma_ws_memory_init(tmp, TCP_GRO_CONTEXT_SIZE, 0);
	cdma_write(snic_params[id].tcp_gro_ctx, tmp, TCP_GRO_CONTEXT_SIZE);

	/* clear GRO stats */
	cdma_write(snic_tcp_gro_param[id].stats_addr, tmp,
		   sizeof(struct tcp_gro_stats_cntrs));
}

__HOT_CODE static inline void snic_tcp_gro(uint16_t snic_id,
			struct fdma_queueing_destination_params *enqueue_params)
{
	uint64_t tcp_gro_ctx = snic_params[snic_id].tcp_gro_ctx;
	struct tcp_gro_context_params *params = &snic_tcp_gro_param[snic_id];

	int status = tcp_gro_aggregate_seg(tcp_gro_ctx, params,
					   TCP_GRO_CALCULATE_TCP_CHECKSUM |
					   TCP_GRO_CALCULATE_IP_CHECKSUM |
					   TCP_GRO_USE_HWC_SPID);
	if (status < 0) {
		fdma_discard_default_frame(FDMA_DIS_FRAME_TC_BIT);
		fdma_terminate_task();
	}

	snic_set_enqueue_param(snic_id, enqueue_params);
	if (((status & TCP_GRO_SEG_AGG_DONE) == TCP_GRO_SEG_AGG_DONE) ||
	    ((status & TCP_GRO_SEG_AGG_DONE_AGG_OPEN) == TCP_GRO_SEG_AGG_DONE_AGG_OPEN))
		snic_send(enqueue_params, FDMA_DIS_NO_FLAGS);

	if ((status & TCP_GRO_FLUSH_REQUIRED) == TCP_GRO_FLUSH_REQUIRED) {
		status = tcp_gro_flush_aggregation(tcp_gro_ctx);
		if (status == TCP_GRO_FLUSH_AGG_DONE)
			snic_send(enqueue_params, FDMA_DIS_NO_FLAGS);
	}

	fdma_terminate_task();
}

__HOT_CODE static inline void snic_tcp_gso(uint16_t snic_id,
			struct fdma_queueing_destination_params *enqueue_params)
{
#define TCP_MSS 1220
	uint32_t total_length = (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
	tcp_gso_ctx_t tcp_gso_ctx;
	int status;
	int err;

	if (total_length <= TCP_MSS)
		return;

	snic_set_enqueue_param(snic_id, enqueue_params);
	tcp_gso_context_init(0, TCP_MSS, tcp_gso_ctx);

	do {
		status = tcp_gso_generate_seg(tcp_gso_ctx);
		if (status == TCP_GSO_GEN_SEG_STATUS_SYN_RST_SET)
			return;

		err = snic_send(enqueue_params, FDMA_DIS_NO_FLAGS);
		if (err)
		{
			if (status == TCP_GSO_GEN_SEG_STATUS_IN_PROCESS)
				tcp_gso_discard_frame_remainder(tcp_gso_ctx);
			break;
		}
	} while (status == TCP_GSO_GEN_SEG_STATUS_IN_PROCESS);

	fdma_terminate_task();
	return;
#undef TCP_MSS
}

__HOT_CODE ENTRY_POINT void snic_process_packet(void)
{
	struct parse_result *pr;
	struct fdma_queueing_destination_params enqueue_params;
	int32_t parse_status;
	uint16_t snic_id;
#if defined(ENABLE_SNIC_IPSEC) || defined(ENABLE_SNIC_VLAN)
	uint16_t asa_length;
#endif
#ifdef ENABLE_SNIC_OSM
	struct scope_status_params scope_status;
#endif

	/* get sNIC ID */
	snic_id = SNIC_ID_GET;
	ASSERT_COND(snic_id < MAX_SNIC_NO);

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);

	osm_task_init();
	/* todo: prpid=0?, starting HXS=0?*/
	*((uint8_t *)HWC_SPID_ADDRESS) = snic_params[snic_id].spid;
	default_task_params.parser_profile_id = SNIC_PRPID;
	default_task_params.parser_starting_hxs = SNIC_HXS;

	parse_status = parse_result_generate_default(PARSER_NO_FLAGS);
	if (parse_status) {
		pr_err("HF-NIC[%d]: parser status: 0x%x\n", snic_id, parse_status);
		fdma_discard_default_frame(FDMA_DIS_FRAME_TC_BIT);
	}

	/* Check IP & TCP */
	if (!(PARSER_IS_IP_DEFAULT() && PARSER_IS_TCP_DEFAULT())) {
	}
	else if (SNIC_IS_INGRESS_GET) {
		/* snic uses only 1 QDID so we need to have different
		 * qd/priority for ingress than for egress */
		default_task_params.qd_priority = 8;

		snic_tcp_gro(snic_id, &enqueue_params);

#ifdef ENABLE_SNIC_IPR
		/* For ingress may need to do IPR and then Remove Vlan */
		if (snic->snic_enable_flags & SNIC_IPR_EN)
			snic_ipr(snic);
#endif

#ifdef ENABLE_SNIC_VLAN
		/*reach here if re-assembly success or regular or IPR disabled*/
		if (snic->snic_enable_flags & SNIC_VLAN_REMOVE_EN)
			l2_pop_vlan();
#endif

#ifdef ENABLE_SNIC_IPSEC
		/* Check if ipsec transport mode is required */
		if (snic->snic_enable_flags & SNIC_IPSEC_EN)
			snic_ipsec_decrypt(snic);
#endif
	}
	/* Egress*/
	else {
		default_task_params.qd_priority = ((*((uint8_t *)
				(HWC_ADC_ADDRESS +
				ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);

		snic_tcp_gso(snic_id, &enqueue_params);

#if defined(ENABLE_SNIC_IPSEC) || defined(ENABLE_SNIC_VLAN)
		/* epid defaults is not present ASA */
		if ((snic->snic_enable_flags & SNIC_VLAN_ADD_EN) ||
			(snic->snic_enable_flags & SNIC_IPSEC_EN))
		{
			fdma_read_default_frame_asa((void*)SNIC_ASA_LOCATION, 0,
					SNIC_ASA_SIZE, &asa_length);
		}
#endif

#ifdef ENABLE_SNIC_IPSEC
		/* Check if ipsec transport mode is required */
		if (snic->snic_enable_flags & SNIC_IPSEC_EN)
			snic_ipsec_encrypt(snic);
#endif

#ifdef ENABLE_SNIC_VLAN
		/* For Egress may need to do add Vlan and then IPF */
		if (snic->snic_enable_flags & SNIC_VLAN_ADD_EN)
			snic_add_vlan();
#endif

#ifdef ENABLE_SNIC_IPF
		if ((snic->snic_enable_flags & SNIC_IPF_EN)
			&& PARSER_IS_IP_DEFAULT())
				snic_ipf(snic);
#endif
	}

	snic_set_enqueue_param(snic_id, &enqueue_params);

#ifdef ENABLE_SNIC_OSM
	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	if (scope_status.scope_mode == CONCURRENT) 
	{
		/* change to exclusive ordering mode */
		osm_scope_transition_to_exclusive_with_increment_scope_id();
	}
#endif

	/* error cases */
	snic_send(&enqueue_params, FDMA_DIS_FRAME_TC_BIT);
	fdma_terminate_task();
}

#ifdef ENABLE_SNIC_IPF
/* Assuming IPF is the last iteration before enqueue (IPF after encryption)*/
int snic_ipf(struct snic_params *snic)
{
	uint16_t ip_offset;
	uint32_t total_length;
	struct ipv4hdr *ipv4_hdr;
	struct ipv6hdr *ipv6_hdr;
	ipf_ctx_t ipf_context_addr;
	int32_t ipf_status;
	int err;
	struct fdma_queueing_destination_params enqueue_params;
	struct scope_status_params scope_status;

	ip_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4_hdr = (struct ipv4hdr *)
			(ip_offset + PRC_GET_SEGMENT_ADDRESS());
	/* need to check frame size against MTU */
	if (PARSER_IS_OUTER_IPV6_DEFAULT())
	{
		ipv6_hdr = (struct ipv6hdr *)ipv4_hdr;
		total_length =
			(uint32_t)(ipv6_hdr->payload_length
					+ 40);
	}
	else
		total_length = (uint32_t)ipv4_hdr->total_length;

	if (total_length > snic->snic_ipf_mtu)
	{
		/* for the enqueue set hash from TLS, an flags equal 0 meaning \
		 * that the qd_priority is taken from the TLS and that enqueue \
		 * function always returns*/
		enqueue_params.qdbin = 0;
		enqueue_params.qd = snic->qdid;
		enqueue_params.qd_priority = default_task_params.qd_priority;
		ipf_context_init(0, snic->snic_ipf_mtu,
				ipf_context_addr);

		do {
			/* todo: error cases when IPF FM will support fdma errors*/
			ipf_status = ipf_generate_frag(ipf_context_addr);
			if (ipf_status == IPF_GEN_FRAG_STATUS_DF_SET)
			{
				pr_err("HF-NIC[%d]: IPF status: 0x%x\n", SNIC_ID_GET, ipf_status);
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
				break;
			}
			/* transition to exclusive just before last fragment enqueue */
			if (ipf_status == IPF_GEN_FRAG_STATUS_DONE)
			{
				/* Get OSM status (ordering scope mode and levels) */
				osm_get_scope(&scope_status);

				if (scope_status.scope_mode == CONCURRENT) 
				{
					/* change to exclusive ordering mode */
					osm_scope_transition_to_exclusive_with_increment_scope_id();
				}
			}
			err = fdma_store_and_enqueue_default_frame_qd(&enqueue_params,
					FDMA_ENWF_NO_FLAGS);
			if (err)
			{
				pr_err("HF-NIC[%d]: IPF - fdma store and enqueue error: 0x%x\n", SNIC_ID_GET, err);
				if (err == -ENOMEM)
					fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
				else /* (err == -EBUSY) */
					fdma_discard_fd(
					      (struct ldpaa_fd *)HWC_FD_ADDRESS,
					      0, FDMA_DIS_AS_BIT);
				if (ipf_status == IPF_GEN_FRAG_STATUS_IN_PROCESS)
					ipf_discard_frame_remainder(ipf_context_addr);
				break;
			}
			
		} while (ipf_status == IPF_GEN_FRAG_STATUS_IN_PROCESS);

		fdma_terminate_task();
		return 0;
	}
	else
		return 0;
}
#endif	/* ENABLE_SNIC_IPF */

#ifdef ENABLE_SNIC_IPR
int snic_ipr(struct snic_params *snic)
{
	int32_t reassemble_status;

	reassemble_status = ipr_reassemble(snic->ipr_instance_val);
	if (reassemble_status != IPR_REASSEMBLY_REGULAR &&
		reassemble_status != IPR_REASSEMBLY_SUCCESS)
	{
		/* todo: error cases*/
		if (reassemble_status != IPR_REASSEMBLY_NOT_COMPLETED)
			pr_err("HF-NIC[%d]: IPR status: 0x%x\n", SNIC_ID_GET, reassemble_status);
		fdma_terminate_task();
		return 0;
	}

	else
		return 0;
}
#endif	/* ENABLE_SNIC_IPR */

#ifdef ENABLE_SNIC_VLAN
int snic_add_vlan(void)
{
	uint32_t vlan;
	uint32_t asa_seg_addr;	/* ASA Segment Address */

	/* Get ASA pointer */
	asa_seg_addr = (uint32_t)SNIC_ASA_LOCATION;
	vlan = *((uint32_t *)(PTR_MOVE(asa_seg_addr, FASWO1)));
	l2_push_and_set_vlan(vlan);
	return 0;
}
#endif	/* ENABLE_SNIC_VLAN */

#ifdef ENABLE_SNIC_IPSEC
int snic_ipsec_decrypt(struct snic_params *snic)
{
	int sr_status;
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	ipsec_handle_t ipsec_handle;
	uint32_t dec_status;
	
	/* check if ipsec and if not table miss then decrypt */
	if (PARSER_IS_IPSEC_DEFAULT())
	{
		if (PARSER_IS_OUTER_IPV4_DEFAULT())
		{
			sr_status = table_lookup_by_keyid_default_frame(
				TABLE_ACCEL_ID_CTLU,
				(uint16_t)snic->dec_ipsec_ipv4_table_id,
				snic->dec_ipsec_ipv4_key_id,
				&lookup_result);
		}
		else
		{
			sr_status = table_lookup_by_keyid_default_frame(
				TABLE_ACCEL_ID_CTLU,
				(uint16_t)snic->dec_ipsec_ipv6_table_id,
				snic->dec_ipsec_ipv6_key_id,
				&lookup_result);
		}

		if (sr_status == TABLE_STATUS_SUCCESS) 
		{
			/* Hit */
			ipsec_handle = lookup_result.data0;
			ipsec_frame_decrypt(ipsec_handle,
					&dec_status);
			/*todo what happens in case decrypt is not successful*/
		}
	}
	return 0;
}

int snic_ipsec_encrypt(struct snic_params *snic)
{
	uint8_t sa_id;
	uint32_t asa_seg_addr;	/* ASA Segment Address */
	int sr_status;
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	ipsec_handle_t ipsec_handle;
	uint32_t enc_status;
	union table_lookup_key_desc key_desc  __attribute__((aligned(16)));

	/* Get ASA pointer */
	asa_seg_addr = (uint32_t)SNIC_ASA_LOCATION;
	sa_id = *((uint8_t *)(PTR_MOVE(asa_seg_addr, FASWO2)));
	
	key_desc.em_key = &sa_id;
	sr_status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU,
				        (uint16_t)snic->ipsec_table_id,
				        key_desc,
				        1,
				        &lookup_result);

	if (sr_status == TABLE_STATUS_SUCCESS) 
	{
		/* Hit */
		ipsec_handle = lookup_result.data0;
		ipsec_frame_encrypt(ipsec_handle,
				&enc_status);
		/*todo what happens in case encrypt is not successful*/
	}
	
	return 0;
}
#endif	/* ENABLE_SNIC_IPSEC */

static int snic_open_cb(void *dev)
{
	/* TODO: */
	UNUSED(dev);
	return 0;
}


static int snic_close_cb(void *dev)
{
	/* TODO: */
	UNUSED(dev);
	return 0;
}

__COLD_CODE static int snic_ctrl_cb(void *dev, uint16_t cmd, uint32_t size, void *data)
{
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;
	uint16_t snic_id = 0xFFFF, ipf_mtu, snic_flags, qdid, spid;
	int i;
	struct snic_cmd_data *cmd_data = (struct snic_cmd_data *)data;
	struct ipr_params ipr_params = {0};
	struct ipr_params *cfg = &ipr_params;
	uint32_t snic_ep_pc;
	int err = 0;

	UNUSED(dev);
	UNUSED(size);

	switch(cmd)
	{
	case SNIC_IPR_CREATE_INSTANCE:
#ifdef ENABLE_SNIC_IPR
		SNIC_IPR_CREATE_INSTANCE_CMD(SNIC_CMD_READ);
		ipr_params.tmi_id = snic_tmi_id;
		ipr_params.ipv4_timeout_cb = snic_ipr_timout_cb;
		ipr_params.ipv6_timeout_cb = snic_ipr_timout_cb;
		ipr_params.flags |= IPR_MODE_TABLE_LOCATION_PEB; /* timeout mode is between fragments */
		ipr_params.timeout_value_ipv4 = 0xffe0; /* 10ms units */
		ipr_params.timeout_value_ipv6 = 0xffe0; /* 10ms units */
		err = ipr_create_instance(&ipr_params,
				ipr_instance_ptr);
		snic_params[snic_id].ipr_instance_val = ipr_instance;
#endif
		return err;
	case SNIC_IPR_DELETE_INSTANCE:
#ifdef ENABLE_SNIC_IPR
		/* todo: parameters to ipr_delete_instance */
		SNIC_IPR_DELETE_INSTANCE_CMD(SNIC_CMD_READ);
		err = ipr_delete_instance(snic_params[snic_id].ipr_instance_val,
				snic_ipr_confirm_delete_cb, NULL);
#endif
		return err;
	case SNIC_SET_MTU:
		SNIC_CMD_MTU(SNIC_CMD_READ);
		snic_params[snic_id].snic_ipf_mtu = ipf_mtu;
		return 0;
	case SNIC_ENABLE_FLAGS:
		SNIC_ENABLE_FLAGS_CMD(SNIC_CMD_READ);
		snic_params[snic_id].snic_enable_flags = snic_flags;
		return 0;
	case SNIC_SET_QDID:
		SNIC_SET_QDID_CMD(SNIC_CMD_READ);
		snic_params[snic_id].qdid = qdid;
		return 0;
	case SNIC_SET_SPID:
		SNIC_SET_SPID_CMD(SNIC_CMD_READ);
		snic_params[snic_id].spid = (uint8_t)spid;
		return 0;
	case SNIC_REGISTER:
		snic_ep_pc = (uint32_t)snic_process_packet;
		for (i=0; i < MAX_SNIC_NO; i++)
		{
			if (!snic_params[i].valid)
			{
				snic_params[i].valid = TRUE;
				snic_id = (uint16_t)i;
				snic_reset_tcp_gro_ctx(snic_id);
				break;
			}
		}
		SNIC_REGISTER_CMD(SNIC_RSP_PREP);
		fdma_modify_default_segment_data(0, SNIC_CMDSZ_REGISTER);
		if (i == MAX_SNIC_NO)
			return -ENAVAIL;
		else
			return 0;
	case SNIC_UNREGISTER:
		SNIC_UNREGISTER_CMD(SNIC_CMD_READ);
		memset(&snic_params[snic_id], 0, sizeof(struct snic_params));
		return 0;
	case SNIC_IPSEC_CREATE_INSTANCE:
#ifdef ENABLE_SNIC_IPSEC
		err = snic_ipsec_create_instance(cmd_data);
#endif
		return err;

		/* This command must be after setting SPID in the SNIC params*/
	case SNIC_IPSEC_DEL_INSTANCE:
#ifdef ENABLE_SNIC_IPSEC
		err = snic_ipsec_del_instance(cmd_data);
#endif
		return err;
	case SNIC_IPSEC_ADD_SA:
#ifdef ENABLE_SNIC_IPSEC
		if (PRC_GET_SEGMENT_LENGTH() < SNIC_CMDSZ_IPSEC_ADD_SA)
		{	
			fdma_close_default_segment();
			fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS, (void *)PRC_GET_SEGMENT_ADDRESS(), 
					PRC_GET_SEGMENT_OFFSET(), SNIC_CMDSZ_IPSEC_ADD_SA);
		}
		err = snic_ipsec_add_sa(cmd_data);
#endif
		return err;

	case SNIC_IPSEC_DEL_SA:
#ifdef ENABLE_SNIC_IPSEC
		err = snic_ipsec_del_sa(cmd_data);
#endif
		return err;
		
	case SNIC_IPSEC_SA_GET_STATS:
#ifdef ENABLE_SNIC_IPSEC
		err = snic_ipsec_sa_get_stats(cmd_data);
#endif
		return err;

	default:
		return -EINVAL;
	}
	return 0;
}

#ifdef ENABLE_SNIC_IPSEC
int snic_ipsec_create_instance(struct snic_cmd_data *cmd_data)
{
	struct snic_ipsec_cfg snic_ipsec_cfg;
	struct snic_ipsec_cfg *cfg = &snic_ipsec_cfg;
	ipsec_instance_handle_t ws_instance_handle;
	int err;
	uint32_t num_sa;
	uint8_t fec_no, key_size;
	int i;
	uint8_t fec_array[8];
	uint16_t snic_id;
	uint16_t table_id;
	
	SNIC_IPSEC_CREATE_INSTANCE_CMD(SNIC_CMD_READ);
	num_sa = (uint32_t)((cfg->num_sa_ipv4 + cfg->num_sa_ipv6) << 1);
	err =  ipsec_create_instance(num_sa, num_sa,
			0, snic_tmi_id, &ws_instance_handle);
	if (err)
		return err;
	snic_params[snic_id].ipsec_instance_val = ws_instance_handle;
	if (cfg->num_sa_ipv4)
	{
		key_size = 4;
		fec_no = 1;
		i=0;
		if (cfg->sa_selectors & SNIC_IPSEC_INCLUDE_IP_SRC_IN_SA_SELECT)
		{
			key_size += 4;
			fec_no++;
			fec_array[i++] = KEYGEN_KCR_IPSRC_1_FECID;
		}
		if (cfg->sa_selectors & SNIC_IPSEC_INCLUDE_IP_DST_IN_SA_SELECT)
		{
			key_size += 4;
			fec_no++;
			fec_array[i++] = KEYGEN_KCR_IPDST_1_FECID;
		}
		fec_array[i] = KEYGEN_KCR_IPSECSPI_FECID;
		snic_params[snic_id].ipsec_ipv4_key_size = key_size;
		/* create keyid and tableid for decapsulation */
		err = snic_create_table_key_id(fec_no, fec_array, key_size,
			cfg->num_sa_ipv4, cfg->num_sa_ipv4,
			&snic_params[snic_id].dec_ipsec_ipv4_key_id,
			&table_id);
		snic_params[snic_id].dec_ipsec_ipv4_table_id = (uint8_t)table_id;
		if (err)
			return err;
		snic_params[snic_id].ipsec_flags |= SNIC_IPSEC_IPV4_ENABLE;
	}
	if (cfg->num_sa_ipv6)
	{
		key_size = 4;
		fec_no = 1;
		i=0;
		if (cfg->sa_selectors & SNIC_IPSEC_INCLUDE_IP_SRC_IN_SA_SELECT)
		{
			key_size += 16;
			fec_no++;
			fec_array[i++] = KEYGEN_KCR_IPSRC_1_FECID;
		}
		if (cfg->sa_selectors & SNIC_IPSEC_INCLUDE_IP_DST_IN_SA_SELECT)
		{
			key_size += 16;
			fec_no++;
			fec_array[i++] = KEYGEN_KCR_IPDST_1_FECID;
		}
		fec_array[i] = KEYGEN_KCR_IPSECSPI_FECID;
		snic_params[snic_id].ipsec_ipv6_key_size = key_size;
		/* create key-id and table-id for de-capsulation */
		err = snic_create_table_key_id(fec_no, fec_array, key_size,
			cfg->num_sa_ipv6, cfg->num_sa_ipv6,
			&snic_params[snic_id].dec_ipsec_ipv6_key_id,
			&table_id);
		snic_params[snic_id].dec_ipsec_ipv6_table_id = (uint8_t)table_id;
		if (err)
			return err;
		snic_params[snic_id].ipsec_flags |= SNIC_IPSEC_IPV6_ENABLE;
	}
	/* create tableid for SA ID management */
	err = snic_create_table_key_id(0, NULL, 1,
			num_sa, num_sa,
			NULL,
			&table_id);
	snic_params[snic_id].ipsec_table_id = (uint8_t)table_id;
	return err;
}

int snic_ipsec_add_sa(struct snic_cmd_data *cmd_data)
{
	struct snic_ipsec_sa_cfg snic_ipsec_sa_cfg;
	struct snic_ipsec_sa_cfg *cfg = &snic_ipsec_sa_cfg;
	struct ipsec_descriptor_params ipsec_params = {0};
	struct ipsec_descriptor_params *ipsec_cfg = &ipsec_params;
	uint8_t sa_id;
	uint16_t snic_id;
	uint64_t rule_id;

	int i, k, err;
	uint16_t outer_hdr_size;
	/* Max: SPI, IPv6 src + dest is 36 bytes */
	uint8_t ipsec_dec_key[36];
	/* IPv6 header plus 96 bytes options gives 136*/
	uint8_t outer_header[136];
	uint16_t options = 0;
	uint32_t key_enc_flags = 0;
	uint32_t nic_options;
	ipsec_handle_t ipsec_handle;
	uint32_t flags = 0;
	struct table_rule rule
					__attribute__((aligned(16)));

	SNIC_IPSEC_ADD_SA_CMD(SNIC_CMD_READ, SNIC_CMD_READ_BYTE_ARRAY);
	if (cfg->options & SNIC_IPSEC_SA_OPT_EXT_SEQ_NUM)
		options |= IPSEC_OPTS_ESP_ESN;
	if (cfg->options & SNIC_IPSEC_SA_OPT_IPV6)
		options |= IPSEC_OPTS_ESP_IPVSN;
	if (cfg->direction == SNIC_IPSEC_SA_IN)
	{
		ipsec_cfg->direction = IPSEC_DIRECTION_INBOUND;
		if (cfg->in.anti_replay != 
				SNIC_IPSEC_SA_ANTI_REPLAY_NONE)
		{
			if (cfg->in.anti_replay == SNIC_IPSEC_SA_ANTI_REPLAY_WS_32)
				options |= IPSEC_DEC_OPTS_ARS32;
			else
			{
				if (cfg->in.anti_replay == SNIC_IPSEC_SA_ANTI_REPLAY_WS_64)
					options |= IPSEC_DEC_OPTS_ARS64;
				else
					options |= IPSEC_DEC_OPTS_ARS128;
			}
			if (cfg->mode == SNIC_IPSEC_SA_TUNNEL)
			{
				if (cfg->options & SNIC_IPSEC_SA_OPT_TECN)
					options |= IPSEC_DEC_OPTS_TECN;
				if (cfg->options & SNIC_IPSEC_SA_OPT_DECR_INNER_TTL_IN)
					options |= IPSEC_DEC_OPTS_DTTL;
				if (cfg->options & SNIC_IPSEC_SA_OPT_COPY_OUTER_DIFFSERV)
					options |= IPSEC_DEC_OPTS_DSC;
			}
		}
		ipsec_cfg->decparams.options = options;
		ipsec_cfg->decparams.seq_num_ext_hi = cfg->seq_num_ext;
		ipsec_cfg->decparams.seq_num = cfg->seq_num;
		/* it is not important what the exact alg is since it is always array of 4 bytes anyway */
		for (i=0; i < 4; i++)
			ipsec_cfg->decparams.gcm.salt[i]= cfg->cipher.nonce_or_salt[i];
	}
	else
	{
		ipsec_cfg->direction = IPSEC_DIRECTION_OUTBOUND;
		if (cfg->options & SNIC_IPSEC_SA_OPT_RND_GEN_IV)
					options |= IPSEC_ENC_OPTS_IVSRC;
		if (nic_options & SNIC_IPSEC_OPT_SEQ_NUM_ROLLOVER_EVENT)
					options |= IPSEC_ENC_OPTS_SNR_EN;
		if (cfg->mode == SNIC_IPSEC_SA_TUNNEL)
		{
			if (cfg->options & SNIC_IPSEC_SA_OPT_COPY_INNER_DF)
				options |= IPSEC_ENC_OPTS_DFC;
			if (cfg->options & SNIC_IPSEC_SA_OPT_DECR_INNER_TTL_OUT)
				options |= IPSEC_ENC_OPTS_DTTL;
			outer_hdr_size = cfg->out.outer_hdr_size;
			ipsec_cfg->encparams.ip_hdr_len = outer_hdr_size;
			cdma_read(outer_header, cfg->out.outer_hdr_paddr, outer_hdr_size);
			ipsec_cfg->encparams.outer_hdr = (uint32_t *)outer_header;
		}
		ipsec_cfg->encparams.options = options;
		ipsec_cfg->encparams.seq_num_ext_hi = cfg->seq_num_ext;
		ipsec_cfg->encparams.seq_num = cfg->seq_num;
		ipsec_cfg->encparams.spi = cfg->spi;
		/* it is not important what the exact alg */
		for (i=0; i < 16; i++)
			ipsec_cfg->encparams.cbc.iv[i] = cfg->cipher.iv[i];
	}
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_DES_IV64)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_DES_IV64;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_DES)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_DES;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_3DES)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_3DES;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_NULL)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_NULL;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_CBC)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_CBC;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_CTR)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_CTR;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_CCM8)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_CCM8;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_CCM12)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_CCM12;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_CCM16)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_CCM16;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_GCM8)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_GCM8;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_GCM12)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_GCM12;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_GCM16)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_GCM16;
	if (cfg->cipher.alg == SNIC_IPSEC_CIPHER_AES_NULL_WITH_GMAC)
		ipsec_cfg->cipherdata.algtype = IPSEC_CIPHER_AES_NULL_WITH_GMAC;
	ipsec_cfg->cipherdata.keylen = cfg->cipher.key_size;
	ipsec_cfg->cipherdata.key = cfg->cipher.key_paddr;
	if (cfg->options & SNIC_IPSEC_SA_OPT_KEY_ENCRYPT)
		key_enc_flags |= IPSEC_KEY_ENC;
	if (cfg->options & SNIC_IPSEC_SA_OPT_KEY_NO_WR_BCK)
		key_enc_flags |= IPSEC_KEY_NWB;
	if (cfg->options & SNIC_IPSEC_SA_OPT_KEY_ENHANC_ENCRYPT)
		key_enc_flags |= IPSEC_KEY_EKT;
	if (cfg->options & SNIC_IPSEC_SA_OPT_KEY_TRUST_ENCRYPT)
		key_enc_flags |= IPSEC_KEY_TK;
	ipsec_cfg->cipherdata.key_enc_flags = key_enc_flags;
	
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_NULL)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_NULL;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_HMAC_MD5_96)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_MD5_96;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_HMAC_SHA1_96)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_SHA1_96;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_AES_XCBC_MAC_96)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_AES_XCBC_MAC_96;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_HMAC_MD5_128)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_MD5_128;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_HMAC_SHA1_160)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_SHA1_160;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_AES_CMAC_96)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_AES_CMAC_96;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_HMAC_SHA2_256_128)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_SHA2_256_128;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_HMAC_SHA2_384_192)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_SHA2_384_192;
	if (cfg->auth.alg == SNIC_IPSEC_AUTH_HMAC_SHA2_512_256)
		ipsec_cfg->authdata.algtype = IPSEC_AUTH_HMAC_SHA2_512_256;
	ipsec_cfg->authdata.keylen = cfg->auth.key_size;
	ipsec_cfg->authdata.key = cfg->auth.key_paddr;
	ipsec_cfg->authdata.key_enc_flags = key_enc_flags;

	/* transport mode. IPSEC_FLG_LIFETIME_SEC_CNTR_EN not supported yet */
	if (cfg->mode == SNIC_IPSEC_SA_TUNNEL)
	{
		flags |= IPSEC_FLG_TUNNEL_MODE;
		if (cfg->options & SNIC_IPSEC_SA_OPT_NAT_UDP_CHKS)
			flags |= IPSEC_ENC_OPTS_NUC_EN;
		if (cfg->options & SNIC_IPSEC_SA_OPT_CFG_DSCP)
			flags |= IPSEC_FLG_ENC_DSCP_SET;
	}
	else
	{
		if (cfg->options & SNIC_IPSEC_SA_OPT_PAD_CHECK)
			flags |= IPSEC_FLG_TRANSPORT_PAD_CHECK;
	}
	if (cfg->options & SNIC_IPSEC_SA_OPT_BUFF_REUSE)
		flags |= IPSEC_FLG_BUFFER_REUSE;
	if (cfg->options & SNIC_IPSEC_SA_OPT_NAT) /* tunnel only??? */
		flags |= SNIC_IPSEC_SA_OPT_NAT;
	if (cfg->options & SNIC_IPSEC_SA_OPT_KB_CNT)
		flags |= IPSEC_FLG_LIFETIME_KB_CNTR_EN;
	if (cfg->options & SNIC_IPSEC_SA_OPT_PKT_CNT)
		flags |= IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
	if (cfg->options & SNIC_IPSEC_SA_OPT_SEC_CNT)
		flags |= IPSEC_FLG_LIFETIME_SEC_CNTR_EN;
	ipsec_params.flags = flags;
	
	ipsec_params.soft_kilobytes_limit = cfg->lifetime.soft_kb; 
	ipsec_params.hard_kilobytes_limit = cfg->lifetime.hard_kb;
	ipsec_params.soft_packet_limit = cfg->lifetime.soft_packet;
	ipsec_params.hard_packet_limit = cfg->lifetime.hard_packet;
	ipsec_params.soft_seconds_limit = cfg->lifetime.soft_sec;
	ipsec_params.hard_seconds_limit = cfg->lifetime.hard_sec;

	ipsec_params.lifetime_callback = NULL;
	ipsec_params.callback_arg = NULL;
	ipsec_params.spid = (uint8_t)snic_params[snic_id].spid; /* move to create SA */
	err = ipsec_add_sa_descriptor(
			ipsec_cfg,
			snic_params[snic_id].ipsec_instance_val,
			&ipsec_handle);
	if (err)
		return err;

	/* create rule to bind between sa_id and ipsec_handle */
	rule.options = 0;
	rule.result.type = TABLE_RESULT_TYPE_OPAQUE;
	rule.result.data0 = ipsec_handle;
	rule.key_desc.em.key[0] = sa_id;
	err = table_rule_create(TABLE_ACCEL_ID_CTLU,
	(uint16_t)snic_params[snic_id].ipsec_table_id, &rule, 1, &rule_id);

	if (err)
		return err;
	/* create rule to bind between dec key and ipsec handle */
	if (cfg->direction == SNIC_IPSEC_SA_IN)
	{
		rule.result.data0 =
				ipsec_handle;
		if (cfg->options & SNIC_IPSEC_SA_OPT_IPV6)
		{
			k = 0;
			if (nic_options & SNIC_IPSEC_INCLUDE_IP_SRC_IN_SA_SELECT)
			{
				k = 16;
				for (i = 0; i < 16; i++)
					ipsec_dec_key[i] = cfg->in.ip_src[i];
			}
			if (nic_options & SNIC_IPSEC_INCLUDE_IP_DST_IN_SA_SELECT)
			{
				for (i = 0; i < 16; i++)
					ipsec_dec_key[k + i] = cfg->in.ip_dst[i];
				k += 16;
			}
			ipsec_dec_key[k++] = (uint8_t)(cfg->spi >> 24);
			ipsec_dec_key[k++] = (uint8_t)(cfg->spi >> 16);
			ipsec_dec_key[k++] = (uint8_t)(cfg->spi >> 8);
			ipsec_dec_key[k] = (uint8_t)(cfg->spi);
			for (i = 0; i < snic_params[snic_id].ipsec_ipv6_key_size; i++ )
				rule.key_desc.em.key[i] = ipsec_dec_key[i];
			err = table_rule_create(TABLE_ACCEL_ID_CTLU,
					(uint16_t)snic_params[snic_id].
							dec_ipsec_ipv6_table_id,
					&rule, snic_params[snic_id].
							ipsec_ipv6_key_size,
					&rule_id);
		}
		else
		{
			k = 0;
			if (nic_options & SNIC_IPSEC_INCLUDE_IP_SRC_IN_SA_SELECT)
			{
				k = 4;
				for (i = 0; i < 4; i++)
					ipsec_dec_key[i] = cfg->in.ip_src[i];
			}
			if (nic_options & SNIC_IPSEC_INCLUDE_IP_DST_IN_SA_SELECT)
			{
				for (i = 0; i < 4; i++)
					ipsec_dec_key[k + i] = cfg->in.ip_dst[i];
				k += 4;
			}
			ipsec_dec_key[k++] = (uint8_t)(cfg->spi >> 24);
			ipsec_dec_key[k++] = (uint8_t)(cfg->spi >> 16);
			ipsec_dec_key[k++] = (uint8_t)(cfg->spi >> 8);
			ipsec_dec_key[k] = (uint8_t)(cfg->spi);
			for (i = 0; i < snic_params[snic_id].ipsec_ipv4_key_size; i++)
				rule.key_desc.em.key[i] = ipsec_dec_key[i];
			err = table_rule_create(TABLE_ACCEL_ID_CTLU,
						(uint16_t)snic_params[snic_id].
							dec_ipsec_ipv4_table_id,
						&rule,
						snic_params[snic_id].
							ipsec_ipv4_key_size,
						&rule_id);
		}

		if (err)
			return err;
	}
	return 0;
}

int snic_ipsec_del_sa(struct snic_cmd_data *cmd_data)
{
	uint8_t sa_id;
	struct snic_ipsec_sa_cfg snic_ipsec_sa_cfg;
	struct snic_ipsec_sa_cfg *cfg = &snic_ipsec_sa_cfg;
	/* Max: SPI, IPv6 src + dest is 36 bytes */
	uint8_t ipsec_dec_key[36];
	uint16_t snic_id;
	uint32_t nic_options;
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	union table_lookup_key_desc table_lookup_key_desc  __attribute__((aligned(16)));
	union table_key_desc key_desc __attribute__((aligned(16)));
	ipsec_handle_t ipsec_handle;
	int err, i, k;
	uint32_t spi;

	SNIC_IPSEC_DEL_SA_CMD(SNIC_CMD_READ, SNIC_CMD_READ_BYTE_ARRAY);
	table_lookup_key_desc.em_key = &sa_id;
	err = table_lookup_by_key(TABLE_ACCEL_ID_CTLU,
				  (uint16_t)snic_params[snic_id].ipsec_table_id,
				  table_lookup_key_desc,
				  1,
				  &lookup_result);
	if (err == TABLE_STATUS_SUCCESS) {
		/* Hit */
		ipsec_handle = lookup_result.data0;
		/* need to delete rules from the 2 tables (or one for enc.) */
		key_desc.em.key[0] = sa_id;
		err = table_rule_delete_by_key_desc(TABLE_ACCEL_ID_CTLU,
				(uint16_t)snic_params[snic_id].ipsec_table_id,
				&key_desc,
				1,
				NULL);
		if (err)
			return err;
		/* Need to check if decryption SA need to remove another rule*/
		if (cfg->direction == SNIC_IPSEC_SA_IN)
		{
			if (cfg->options & SNIC_IPSEC_SA_OPT_IPV6)
			{
				k = 0;
				if (nic_options & SNIC_IPSEC_INCLUDE_IP_SRC_IN_SA_SELECT)
				{
					k = 16;
					for (i = 0; i < 16; i++)
						ipsec_dec_key[i] = cfg->in.ip_src[i];
				}
				if (nic_options & SNIC_IPSEC_INCLUDE_IP_DST_IN_SA_SELECT)
				{
					for (i = 0; i < 16; i++)
						ipsec_dec_key[k + i] = cfg->in.ip_dst[i];
					k += 16;
				}
				spi = cfg->spi;
				ipsec_dec_key[k++] = (uint8_t)(spi >> 24);
				ipsec_dec_key[k++] = (uint8_t)(spi >> 16);
				ipsec_dec_key[k++] = (uint8_t)(spi >> 8);
				ipsec_dec_key[k] = (uint8_t)spi;
				for (i = 0; i < snic_params[snic_id].ipsec_ipv6_key_size; i++ )
					key_desc.em.key[i] = ipsec_dec_key[i];
				err = table_rule_delete_by_key_desc(TABLE_ACCEL_ID_CTLU,
					(uint16_t)snic_params[snic_id].dec_ipsec_ipv6_table_id,
					&key_desc,
					snic_params[snic_id].ipsec_ipv6_key_size,
					NULL);
			}
			else
			{
				k = 0;
				if (nic_options & SNIC_IPSEC_INCLUDE_IP_SRC_IN_SA_SELECT)
				{
					k = 4;
					for (i = 0; i < 4; i++)
						ipsec_dec_key[i] = cfg->in.ip_src[i];
				}
				if (nic_options & SNIC_IPSEC_INCLUDE_IP_DST_IN_SA_SELECT)
				{
					for (i = 0; i < 4; i++)
						ipsec_dec_key[k + i] = cfg->in.ip_dst[i];
					k += 4;
				}
				spi = cfg->spi;
				ipsec_dec_key[k++] = (uint8_t)(spi >> 24);
				ipsec_dec_key[k++] = (uint8_t)(spi >> 16);
				ipsec_dec_key[k++] = (uint8_t)(spi >> 8);
				ipsec_dec_key[k] = (uint8_t)spi;
				for (i = 0; i < snic_params[snic_id].ipsec_ipv4_key_size; i++ )
					key_desc.em.key[i] = ipsec_dec_key[i];
				err = table_rule_delete_by_key_desc(TABLE_ACCEL_ID_CTLU,
					(uint16_t)snic_params[snic_id].dec_ipsec_ipv4_table_id,
					&key_desc,
					snic_params[snic_id].ipsec_ipv4_key_size,
					NULL);
			}
			if (err)
				return err;
		}
		err = ipsec_del_sa_descriptor(ipsec_handle);
		return err;
	}
	else
		return err;
}

int snic_ipsec_del_instance(struct snic_cmd_data *cmd_data)
{
	uint16_t snic_id;
	int err;

	SNIC_IPSEC_DELETE_INSTANCE_CMD(SNIC_CMD_READ);
	err = ipsec_delete_instance(snic_params[snic_id].ipsec_instance_val);
	if (err)
		return err;
	/* return  set of keyid and 2 sets of table ids */
	table_delete(TABLE_ACCEL_ID_CTLU,
			(uint16_t)snic_params[snic_id].ipsec_table_id);
	if (snic_params[snic_id].ipsec_flags & SNIC_IPSEC_IPV4_ENABLE)
	{
		table_delete(TABLE_ACCEL_ID_CTLU,
				(uint16_t)snic_params[snic_id].dec_ipsec_ipv4_table_id);
		err = keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU,
				snic_params[snic_id].dec_ipsec_ipv4_key_id);
		if (err)
			return err;
	}
	
	if (snic_params[snic_id].ipsec_flags & SNIC_IPSEC_IPV6_ENABLE)
	{
		table_delete(TABLE_ACCEL_ID_CTLU,
				(uint16_t)snic_params[snic_id].dec_ipsec_ipv6_table_id);
		err = keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU,
				snic_params[snic_id].dec_ipsec_ipv6_key_id);
		if (err)
			return err;
	}

	return 0;
}

int snic_ipsec_sa_get_stats(struct snic_cmd_data *cmd_data)
{
	uint16_t snic_id;
	uint8_t sa_id;
	uint32_t secs;
	uint64_t bytes, packets, dropped_pkts;
	struct snic_params *snic;
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	ipsec_handle_t ipsec_handle;
	union table_lookup_key_desc key_desc  __attribute__((aligned(16)));
	int err;
	
	SNIC_IPSEC_SA_GET_STATS_CMD(SNIC_CMD_READ);
	snic = snic_params + snic_id;
	key_desc.em_key = &sa_id;
	err = table_lookup_by_key(TABLE_ACCEL_ID_CTLU,
				  (uint16_t)snic->ipsec_table_id,
				  key_desc,
				  1,
				  &lookup_result);

	if (err == TABLE_STATUS_SUCCESS) 
	{
		/* Hit */
		ipsec_handle = lookup_result.data0;
		err = ipsec_get_lifetime_stats(ipsec_handle, &bytes, &packets,
					       &dropped_pkts, &secs);
		if (err)
			return err;
		SNIC_IPSEC_SA_GET_STATS_RSP_CMD(SNIC_RSP_PREP);
		fdma_modify_default_segment_data(0, SNIC_CMDSZ_IPSEC_SA_GET_STATS_MAX);
		return 0;
	}
	else
		return err;
}
#endif	/* ENABLE_SNIC_IPSEC */

int aiop_snic_early_init(void)
{
#if defined(ENABLE_SNIC_IPR) || defined(ENABLE_SNIC_IPSEC)
	int err;
#endif

#ifdef ENABLE_SNIC_IPR
	/* reserve IPR buffers */
	err = ipr_early_init(MAX_SNIC_NO, MAX_SNIC_NO * MAX_OPEN_IPR_FRAMES);
	if (err)
		return err;
#endif

#ifdef ENABLE_SNIC_IPSEC
	/* IPsec buffers */
	err = ipsec_early_init(MAX_SNIC_NO, MAX_SNIC_NO * MAX_SA_NO, MAX_SNIC_NO * MAX_SA_NO, 0);
	return err;
#endif
	return 0;
}

int aiop_snic_init(void)
{
	int status, i;
	struct cmdif_module_ops snic_cmd_ops;
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;
	uint64_t size;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	snic_cmd_ops.open_cb = (open_cb_t *)snic_open_cb;
	snic_cmd_ops.close_cb = (close_cb_t *)snic_close_cb;
	snic_cmd_ops.ctrl_cb = (ctrl_cb_t *)snic_ctrl_cb;
	pr_info("sNIC: register with cmdif module!\n");

	status = cmdif_register_module("sNIC", &snic_cmd_ops);
	if (status) {
		pr_info("sNIC:Failed to register with cmdif module!\n");
		return status;
	}
	memset(snic_params, 0, sizeof(snic_params));

	status = fsl_get_mem((SNIC_MAX_NO_OF_TIMERS + 4) * 64, mem_pid,
		    SNIC_MEM_ALIGN, &snic_tmi_mem_base_addr);
	if (status) {
		pr_info("sNIC:Failed to allocate memory for TMI.\n");
		return status;
	}

	/* tmi delete in in snic_free */
	status = tman_create_tmi(snic_tmi_mem_base_addr,
				 SNIC_MAX_NO_OF_TIMERS + 3, &snic_tmi_id);
	if (status) {
		pr_info("sNIC:Failed to create TMI.\n");
		return status;
	}

	size = MAX_SNIC_NO * TCP_GRO_CONTEXT_SIZE;
	status = fsl_get_mem(size, mem_pid, SNIC_MEM_ALIGN, &snic_gro_addr);
	if (status) {
		pr_info("sNIC:Failed to allocate memory for TCP GRO.\n");
		return status;
	}

	memset(snic_tcp_gro_param, 0, sizeof(snic_tcp_gro_param));

	size = sizeof(struct tcp_gro_stats_cntrs);
	if (size < SNIC_MEM_ALIGN)
		size = SNIC_MEM_ALIGN;
	status = fsl_get_mem(size * MAX_SNIC_NO, mem_pid, SNIC_MEM_ALIGN,
			     &snic_gro_stats_addr);
	if (status) {
		pr_info("sNIC:Failed to allocate memory for TCP GRO stats.\n");
		return status;
	}
	for (i = 0; i < MAX_SNIC_NO; i++)
		snic_set_tcp_gro_param(i, snic_gro_stats_addr + i * size,
				       snic_tmi_id);

	return status;
}

void aiop_snic_free(void)
{
	tman_delete_tmi(snic_tman_confirm_cb, TMAN_INS_DELETE_MODE_FORCE_EXP,
			snic_tmi_id, NULL, NULL);
	
}

void snic_tman_confirm_cb(tman_arg_8B_t arg1, tman_arg_2B_t arg2)
{
	UNUSED(arg1);
	UNUSED(arg2);
	tman_timer_completion_confirmation(
			TMAN_GET_TIMER_HANDLE(HWC_FD_ADDRESS));
	fsl_put_mem(snic_tmi_mem_base_addr);
	fsl_put_mem(snic_gro_stats_addr);
	fsl_put_mem(snic_gro_addr);
	fdma_terminate_task();
}

#ifdef ENABLE_SNIC_IPR
void snic_ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);
	/* Need to discard default frame */
	fdma_discard_default_frame(FDMA_DIS_FRAME_TC_BIT);
}

void snic_ipr_confirm_delete_cb(ipr_del_arg_t arg)
{
	UNUSED(arg);
	fdma_terminate_task();
}
#endif	/* ENABLE_SNIC_IPR */

#ifdef ENABLE_SNIC_IPSEC
int snic_create_table_key_id(uint8_t fec_no, uint8_t fec_array[8], 
				uint8_t key_size,
				uint32_t committed_sa_num, uint32_t max_sa_num,
				uint8_t *key_id,
				uint16_t *table_id)
{
	struct kcr_builder kb
			__attribute__((aligned(16)));
	int	err, i;
	struct table_create_params tbl_params;

	if (fec_no)
	{
		keygen_kcr_builder_init(&kb);
		for (i = 0; i < fec_no; i++)
			keygen_kcr_builder_add_protocol_specific_field(
				(enum kcr_builder_protocol_fecid)fec_array[i],
				NULL , &kb);
		
		err = keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU,
				  kb.kcr,
				  key_id);
		if (err < 0)
		{
			return err;
		}
	}

	/*todo these limits are also for egress and we need for ingress only*/
	tbl_params.committed_rules = committed_sa_num;
	tbl_params.max_rules = max_sa_num;
	tbl_params.key_size = key_size;
	tbl_params.attributes = TABLE_ATTRIBUTE_TYPE_EM | \
			TABLE_ATTRIBUTE_LOCATION_PEB | \
			TABLE_ATTRIBUTE_MR_NO_MISS;
	tbl_params.timestamp_accuracy = 1;
	err = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params,
			table_id);
	if (err)
	{
		tbl_params.attributes = TABLE_ATTRIBUTE_TYPE_EM | \
				TABLE_ATTRIBUTE_LOCATION_SYS_DDR | \
				TABLE_ATTRIBUTE_MR_NO_MISS;
		err = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params,
				table_id);
	}
	return err;
}
#endif	/* ENABLE_SNIC_IPSEC */

#endif	/* ENABLE_SNIC */
