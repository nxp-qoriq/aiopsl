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

#include "snic.h"
#include "system.h"
#include "net/fsl_net.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_errors.h"
#include "platform.h"
#include "fsl_io.h"
#include "aiop_common.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"

#include "dplib/fsl_parser.h"
#include "dplib/fsl_l2.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_mc_sys.h"
#include "fsl_mc_cmd.h"
#include "fsl_snic_cmd.h"

#include "general.h"
#include "osm_inline.h"

#include "dplib/fsl_ipf.h"
#include "dplib/fsl_table.h"
#include "fsl_cmdif_server.h"

#include "ls2085_aiop/fsl_platform.h"
#include "fsl_malloc.h"


#define SNIC_CMD_READ(_param, _offset, _width, _type, _arg) \
	_arg = (_type)u64_dec(cmd_data->params[_param], _offset, _width);

#define SNIC_RSP_PREP(_param, _offset, _width, _type, _arg) \
	cmd_data->params[_param] |= u64_enc(_offset, _width, _arg);

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

void snic_process_packet(void)
{

	struct parse_result *pr;
	struct snic_params *snic;
	struct fdma_queueing_destination_params enqueue_params;
	int32_t parse_status;
	uint16_t snic_id;
	int err;

	/* get sNIC ID */
	snic_id = SNIC_ID_GET;
	ASSERT_COND(snic_id < MAX_SNIC_NO);
	snic = snic_params + snic_id;

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);

	osm_task_init();
	/* todo: prpid=0?, starting HXS=0?*/
	*((uint8_t *)HWC_SPID_ADDRESS) = snic->spid;
	default_task_params.parser_profile_id = SNIC_PRPID;
	default_task_params.parser_starting_hxs = SNIC_HXS;

	parse_status = parse_result_generate_default(PARSER_NO_FLAGS);
	if (parse_status){
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}

	if (SNIC_IS_INGRESS_GET) {
		/* snic uses only 1 QDID so we need to have different
		 * qd/priority for ingress than for egress */
		default_task_params.qd_priority = 8;
		/* For ingress may need to do IPR and then Remove Vlan */
		if (snic->snic_enable_flags & SNIC_IPR_EN)
			snic_ipr(snic);
		/*reach here if re-assembly success or regular or IPR disabled*/
		if (snic->snic_enable_flags & SNIC_VLAN_REMOVE_EN)
			l2_pop_vlan();
		/* Check if ipsec transport mode is required */
		if (snic->snic_enable_flags & SNIC_IPSEC_DECRYPT)
			snic_ipsec_decrypt(snic);
	}
	/* Egress*/
	else {
		default_task_params.qd_priority = ((*((uint8_t *)
				(HWC_ADC_ADDRESS +
				ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);
		/* Check if ipsec transport mode is required */
		if (snic->snic_enable_flags & SNIC_IPSEC_ENCRYPT)
			snic_ipsec_encrypt(snic);
		/* For Egress may need to do add Vlan and then IPF */
		if (snic->snic_enable_flags & SNIC_VLAN_ADD_EN)
			snic_add_vlan();

		if (snic->snic_enable_flags & SNIC_IPF_EN)
			snic_ipf(snic);
	}

	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.qdbin = 0;
	enqueue_params.qd = snic->qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	/* error cases */
	err = fdma_store_and_enqueue_default_frame_qd(&enqueue_params, \
			FDMA_ENWF_NO_FLAGS);
	if(err == -ENOMEM)
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	/* todo other error case */
	fdma_terminate_task();
}


/* Assuming IPF is the last iteration before enqueue (IPF after encryption)*/
int snic_ipf(struct snic_params *snic)
{
	uint16_t ip_offset;
	uint32_t total_length;
	struct ipv4hdr *ipv4_hdr;
	struct ipv6hdr *ipv6_hdr;
	ipf_ctx_t ipf_context_addr
		__attribute__((aligned(sizeof(struct ldpaa_fd))));
	int32_t ipf_status;
	int err;
	struct fdma_queueing_destination_params enqueue_params;

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
			ipf_status = ipf_generate_frag(ipf_context_addr);
			/* todo error cases */
			err = fdma_store_and_enqueue_default_frame_qd(&enqueue_params,
					FDMA_ENWF_NO_FLAGS);
			if(err == -ENOMEM)
			{
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
				if (ipf_status == IPF_GEN_FRAG_STATUS_IN_PROCESS)
					ipf_discard_frame_remainder(ipf_context_addr);
				break;
			}
			/* todo other error case */
		} while (ipf_status == IPF_GEN_FRAG_STATUS_IN_PROCESS);

		fdma_terminate_task();
		return 0;
	}
	else
		return 0;
}

int snic_ipr(struct snic_params *snic)
{
	int32_t reassemble_status;

	reassemble_status = ipr_reassemble(snic->ipr_instance_val);
	if (reassemble_status != IPR_REASSEMBLY_REGULAR &&
		reassemble_status != IPR_REASSEMBLY_SUCCESS)
	{
		/* todo: error cases*/
		fdma_terminate_task();
		return 0;
	}

	else
		return 0;
}

int snic_add_vlan(void)
{
	uint32_t vlan;
	struct presentation_context *presentation_context;
	uint32_t asa_seg_addr;	/* ASA Segment Address */

	/* Get ASA pointer */
	presentation_context =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	asa_seg_addr = (uint32_t)(presentation_context->
			asapa_asaps & PRC_ASAPA_MASK);
	vlan = *((uint32_t *)(PTR_MOVE(asa_seg_addr, 0x50)));
	l2_push_and_set_vlan(vlan);
	return 0;
}

int snic_ipsec_decrypt(struct snic_params *snic)
{
	int sr_status;
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	ipsec_handle_t ipsec_handle;
	uint32_t dec_status;
	
	/* check if ipsec and if not table miss then decrypt */
	if (PARSER_IS_IPSEC_DEFAULT())
	{
		sr_status = table_lookup_by_keyid_default_frame(
			TABLE_ACCEL_ID_CTLU,
			snic->dec_ipsec_table_id,
			snic->dec_ipsec_key_id,
			&lookup_result);
		if (sr_status == TABLE_STATUS_SUCCESS) 
		{
			/* Hit */
			ipsec_handle = lookup_result.opaque0_or_reference;
			ipsec_frame_decrypt(ipsec_handle,
					&dec_status);
			/*todo what happens in case decrypt is not successful*/
		}
	}
	return 0;
}

int snic_ipsec_encrypt(struct snic_params *snic)
{
	uint16_t sa_id;
	struct presentation_context *presentation_context;
	uint32_t asa_seg_addr;	/* ASA Segment Address */
	int sr_status;
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	ipsec_handle_t ipsec_handle;
	uint32_t enc_status;
	union table_lookup_key_desc key_desc  __attribute__((aligned(16)));

	/* Get ASA pointer */
	presentation_context =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	asa_seg_addr = (uint32_t)(presentation_context->
			asapa_asaps & PRC_ASAPA_MASK);
	sa_id = *((uint16_t *)(PTR_MOVE(asa_seg_addr, 0x54)));
	
	key_desc.em_key = &sa_id;
	sr_status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU, snic->ipsec_table_id, key_desc, 2, &lookup_result);

	if (sr_status == TABLE_STATUS_SUCCESS) 
	{
		/* Hit */
		ipsec_handle = lookup_result.opaque0_or_reference;
		ipsec_frame_encrypt(ipsec_handle,
				&enc_status);
		/*todo what happens in case encrypt is not successful*/
	}
	
	return 0;
}

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
	struct ipsec_descriptor_params ipsec_params = {0};
	struct ipsec_encap_params encparams = {0};
	struct ipsec_decap_params decparams = {0};
	struct ipsec_encap_cbc_params encap_cbc = {0};
	struct ipsec_encap_ctr_params encap_ctr = {0};
	struct ipsec_encap_ccm_params encap_ccm = {0};
	struct ipsec_encap_gcm_params encap_gcm = {0};
	struct ipsec_decap_ctr_params decap_ctr = {0};
	struct ipsec_decap_ccm_params decap_ccm = {0};
	struct ipsec_decap_gcm_params decap_gcm = {0};
	/* should get value from: enum ipsec_cipher_type */
	uint8_t cipher_type;
	struct ipsec_descriptor_params *ipsec_cfg = &ipsec_params;
	struct ipsec_encap_params *ipsec_encparams_cfg = &encparams;
	struct ipsec_decap_params *ipsec_decparams_cfg = &decparams;
	struct ipsec_encap_cbc_params *ipsec_encap_cbc_cfg = &encap_cbc;
	struct ipsec_encap_ctr_params *ipsec_encap_ctr_cfg = &encap_ctr;
	struct ipsec_encap_ccm_params *ipsec_encap_ccm_cfg = &encap_ccm;
	struct ipsec_encap_gcm_params *ipsec_encap_gcm_cfg = &encap_gcm;
	struct ipsec_decap_ctr_params *ipsec_decap_ctr_cfg = &decap_ctr;
	struct ipsec_decap_ccm_params *ipsec_decap_ccm_cfg = &decap_ccm;
	struct ipsec_decap_gcm_params *ipsec_decap_gcm_cfg = &decap_gcm;
	uint32_t snic_ep_pc;
	int err = 0;
	uint32_t committed_sa_num, max_sa_num;
	uint32_t dec_committed_sa_num, dec_max_sa_num;
	uint32_t enc_committed_sa_num, enc_max_sa_num;
	ipsec_instance_handle_t ws_instance_handle = 0;
	uint8_t fec_no, key_size;
	uint8_t fec_array[8];
	uint32_t table_location;
	uint16_t sa_id;
	/* example: SPI, IP dest. and protocol */
	uint8_t ipsec_dec_key[48];
	ipsec_handle_t ipsec_handle = 0;
	struct table_rule rule
					__attribute__((aligned(16)));
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	union table_lookup_key_desc table_lookup_key_desc  __attribute__((aligned(16)));
	union table_key_desc key_desc __attribute__((aligned(16)));
	int32_t direction;

	UNUSED(dev);

	switch(cmd)
	{
	case SNIC_IPR_CREATE_INSTANCE:
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
		return err;
	case SNIC_IPR_DELETE_INSTANCE:
		/* todo: parameters to ipr_delete_instance */
		SNIC_IPR_DELETE_INSTANCE_CMD(SNIC_CMD_READ);
		err = ipr_delete_instance(snic_params[snic_id].ipr_instance_val,
				snic_ipr_confirm_delete_cb, NULL);
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
				break;
			}
		}
		SNIC_REGISTER_CMD(SNIC_RSP_PREP);
		fdma_modify_default_segment_data(0, (uint16_t)size);
		if (i == MAX_SNIC_NO)
			return -ENAVAIL;
		else
			return 0;
	case SNIC_UNREGISTER:
		SNIC_UNREGISTER_CMD(SNIC_CMD_READ);
		memset(&snic_params[snic_id], 0, sizeof(struct snic_params));
		return 0;
	case SNIC_IPSEC_CREATE_INSTANCE:
		SNIC_IPSEC_CREATE_INSTANCE_CMD(SNIC_CMD_READ);
		committed_sa_num = dec_committed_sa_num + enc_committed_sa_num;
		max_sa_num = dec_max_sa_num + enc_max_sa_num;
		err =  ipsec_create_instance(committed_sa_num, max_sa_num,
				0, snic_tmi_id, &ws_instance_handle);
		if (err)
			return err;
		snic_params[snic_id].ipsec_instance_val = ws_instance_handle;
		snic_params[snic_id].ipsec_key_size = key_size;
		/* create keyid and tableid for decapsulation*/
		err = snic_create_table_key_id(fec_no, fec_array, key_size,
				table_location, dec_committed_sa_num, dec_max_sa_num,
				&snic_params[snic_id].dec_ipsec_key_id,
				&snic_params[snic_id].dec_ipsec_table_id);
		if (err)
			return err;
		/* create tableid for SA ID management*/
		err = snic_create_table_key_id(0, NULL, key_size,
				table_location, committed_sa_num, max_sa_num,
				NULL,
				&snic_params[snic_id].ipsec_table_id);
		return err;
		/* This command must be after setting SPID in the SNIC params*/
	case SNIC_IPSEC_DELETE_INSTANCE:
		SNIC_IPSEC_DELETE_INSTANCE_CMD(SNIC_CMD_READ);
		err = ipsec_delete_instance(snic_params[snic_id].ipsec_instance_val);
		if (err)
			return err;
		/* return  set of keyid and 2 sets of table ids */
		table_delete(TABLE_ACCEL_ID_CTLU,
				snic_params[snic_id].ipsec_table_id);
		table_delete(TABLE_ACCEL_ID_CTLU,
				snic_params[snic_id].dec_ipsec_table_id);
		err = keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU,
				snic_params[snic_id].dec_ipsec_key_id);
		if (err)
			return err;
		return 0;
		
	case SNIC_IPSEC_ADD_SA:
		SNIC_IPSEC_ADD_SA_CMD(SNIC_CMD_READ);
		if (ipsec_cfg->direction == IPSEC_DIRECTION_OUTBOUND)
		{
			ipsec_cfg->encparams = encparams;
			if (cipher_type == CIPHER_TYPE_CBC)
				ipsec_cfg->encparams.cbc = encap_cbc;
			else if (cipher_type == CIPHER_TYPE_CTR)
				ipsec_cfg->encparams.ctr = encap_ctr;
			else if (cipher_type == CIPHER_TYPE_CCM)
				ipsec_cfg->encparams.ccm = encap_ccm;
			else
				ipsec_cfg->encparams.gcm = encap_gcm;
		}
		else
		{
			ipsec_cfg->decparams = decparams;
			if (cipher_type == CIPHER_TYPE_CCM)
				ipsec_cfg->decparams.ccm = decap_ccm;
			else if (cipher_type == CIPHER_TYPE_CTR)
				ipsec_cfg->decparams.ctr = decap_ctr;
			else
				ipsec_cfg->decparams.gcm = decap_gcm;
		}
		/* transport mode. IPSEC_FLG_LIFETIME_SEC_CNTR_EN not supported yet */
		ipsec_params.flags = IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
		ipsec_params.soft_kilobytes_limit = 0xffffffffffffffff; 
		ipsec_params.hard_kilobytes_limit = 0xffffffffffffffff;
		ipsec_params.soft_packet_limit = 0xffffffffffffffff;
		ipsec_params.hard_packet_limit = 0xffffffffffffffff;
		ipsec_params.soft_seconds_limit = 0x0;
		ipsec_params.hard_seconds_limit = 0x0;

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
		rule.result.type = TABLE_RESULT_TYPE_REFERENCE;
		rule.result.op0_rptr_clp.reference_pointer = ipsec_handle;
		rule.key_desc.em.key[0] = (uint8_t)(sa_id >> 8);
		rule.key_desc.em.key[1] = (uint8_t)sa_id;
		err = table_rule_create(TABLE_ACCEL_ID_CTLU,
				snic_params[snic_id].ipsec_table_id, &rule, 2);
		if (err)
			return err;
		/* create rule to bind between dec key and ipsec handle */
		if (ipsec_params.direction == IPSEC_DIRECTION_INBOUND)
		{
			rule.result.op0_rptr_clp.reference_pointer = 
					ipsec_handle;
			for (i = 0; i < snic_params[snic_id].ipsec_key_size; i++ )
				rule.key_desc.em.key[i] = ipsec_dec_key[i];
			err = table_rule_create(TABLE_ACCEL_ID_CTLU,
					snic_params[snic_id].dec_ipsec_table_id, 
					&rule, snic_params[snic_id].ipsec_key_size);
			if (err)
				return err;
		}
		return 0;
	case SNIC_IPSEC_DEL_SA:
		SNIC_IPSEC_DEL_SA_CMD(SNIC_CMD_READ);
		table_lookup_key_desc.em_key = &sa_id;
		err = table_lookup_by_key(TABLE_ACCEL_ID_CTLU, snic_params[snic_id].ipsec_table_id, table_lookup_key_desc, 2, &lookup_result);
		if (err == TABLE_STATUS_SUCCESS) {
			/* Hit */
			ipsec_handle = lookup_result.opaque0_or_reference;
			/* need to delete rules from the 2 tables (or one for enc.) */
			key_desc.em.key[0] = (uint8_t)(sa_id >> 8);
			key_desc.em.key[1] = (uint8_t)sa_id;
			err = table_rule_delete(TABLE_ACCEL_ID_CTLU,
					snic_params[snic_id].ipsec_table_id,
					&key_desc,
					2,
					NULL);
			if (err)
				return err;
			/* Need to check if decryption SA need to remove another rule*/
			if (direction == IPSEC_DIRECTION_INBOUND)
			{
				for (i = 0; i < snic_params[snic_id].ipsec_key_size; i++ )
					key_desc.em.key[i] = ipsec_dec_key[i];
				err = table_rule_delete(TABLE_ACCEL_ID_CTLU,
						snic_params[snic_id].dec_ipsec_table_id,
						&key_desc,
						snic_params[snic_id].ipsec_key_size,
						NULL);
				if (err)
					return err;
			}
			err = ipsec_del_sa_descriptor(ipsec_handle);
			if (err)
				return err;
			return 0;
		}
		else
			return err;
	default:
		return -EINVAL;
	}
	return 0;
}

int aiop_snic_early_init(void)
{
	int err;

	/* reserve IPR buffers */
	err = ipr_early_init(MAX_SNIC_NO, MAX_OPEN_IPR_FRAMES);
	return err;
}

int aiop_snic_init(void)
{
	int status;
	struct cmdif_module_ops snic_cmd_ops;

	snic_cmd_ops.open_cb = (open_cb_t *)snic_open_cb;
	snic_cmd_ops.close_cb = (close_cb_t *)snic_close_cb;
	snic_cmd_ops.ctrl_cb = (ctrl_cb_t *)snic_ctrl_cb;
	pr_info("sNIC: register with cmdif module!\n");
	status = cmdif_register_module("sNIC", &snic_cmd_ops);
	if(status) {
		pr_info("sNIC:Failed to register with cmdif module!\n");
		return status;
	}
	memset(snic_params, 0, sizeof(snic_params));
	fsl_os_get_mem(SNIC_MAX_NO_OF_TIMERS*64, MEM_PART_DP_DDR, 64, 
			&snic_tmi_mem_base_addr);
	/* tmi delete is in snic_free */
	tman_create_tmi(snic_tmi_mem_base_addr , SNIC_MAX_NO_OF_TIMERS, 
			&snic_tmi_id);
	return 0;
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
	fsl_os_put_mem(snic_tmi_mem_base_addr);
	fdma_terminate_task();
}
void snic_ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);
	/* Need to discard default frame */
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

void snic_ipr_confirm_delete_cb(ipr_del_arg_t arg)
{
	UNUSED(arg);
	fdma_terminate_task();
}

int snic_create_table_key_id(uint8_t fec_no, uint8_t fec_array[8], 
				uint8_t key_size, uint32_t table_location,
				uint32_t committed_sa_num, uint32_t max_sa_num,
				uint8_t *key_id,
				uint16_t *table_id)
{
	struct kcr_builder kb
			__attribute__((aligned(16)));
	int	err, i;
	struct table_create_params tbl_params;
	uint16_t table_location_attr;

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
	if (table_location == IPR_MODE_TABLE_LOCATION_INT)
		table_location_attr = TABLE_ATTRIBUTE_LOCATION_INT;
	else if (table_location == IPR_MODE_TABLE_LOCATION_PEB)
		table_location_attr = TABLE_ATTRIBUTE_LOCATION_PEB;
	else if (table_location == IPR_MODE_TABLE_LOCATION_EXT1)
		table_location_attr = TABLE_ATTRIBUTE_LOCATION_EXT1;
	else
		table_location_attr = TABLE_ATTRIBUTE_LOCATION_EXT2;
	tbl_params.attributes = TABLE_ATTRIBUTE_TYPE_EM | \
			table_location_attr | \
			TABLE_ATTRIBUTE_MR_NO_MISS;
	err = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params,
			table_id);
	if (err != TABLE_STATUS_SUCCESS)
		return err;
	return 0;
}
