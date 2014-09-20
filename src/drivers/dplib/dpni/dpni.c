/* Copyright 2014 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Freescale Semiconductor nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
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
#include <fsl_mc_sys.h>
#include <fsl_mc_cmd.h>
#include <fsl_dpni.h>
#include <fsl_dpni_cmd.h>

static int build_extract_cfg_extention(struct dpkg_profile_cfg *cfg,
                                       struct extract_data *data)
{
	int i, j;
	int offset = 0;
	int param = 1;
	struct {
		enum net_prot prot;
		enum dpkg_extract_from_hdr_type type;
		uint32_t field;
		uint8_t size;
		uint8_t offset;
		uint8_t hdr_index;
		uint8_t constant;
		uint8_t num_of_repeats;
	} u_cfg[DPKG_MAX_NUM_OF_EXTRACTS];

	for (i = 0; i < DPKG_MAX_NUM_OF_EXTRACTS; i++) {
		switch (cfg->extracts[i].type) {
		case DPKG_EXTRACT_FROM_HDR:
			u_cfg[i].prot = cfg->extracts[i].extract.from_hdr.prot;
			u_cfg[i].type = cfg->extracts[i].extract.from_hdr.type;
			u_cfg[i].field =
			        cfg->extracts[i].extract.from_hdr.field;
			u_cfg[i].size = cfg->extracts[i].extract.from_hdr.size;
			u_cfg[i].offset =
			        cfg->extracts[i].extract.from_hdr.offset;
			u_cfg[i].hdr_index =
			        cfg->extracts[i].extract.from_hdr.hdr_index;
			break;
		case DPKG_EXTRACT_FROM_DATA:
			u_cfg[i].size = cfg->extracts[i].extract.from_data.size;
			u_cfg[i].offset =
			        cfg->extracts[i].extract.from_data.offset;
			break;
		case DPKG_EXTRACT_CONSTANT:
			u_cfg[i].constant =
			        cfg->extracts[i].extract.constant.constant;
			u_cfg[i].num_of_repeats =
			        cfg->extracts[i].extract.constant.num_of_repeats;
			break;
		default:
			return -EINVAL;
		}
	}
	data->params[0] |= u64_enc(0, 8, cfg->num_extracts);
	data->params[0] = cpu_to_le64(data->params[0]);

	for (i = 0; i < DPKG_MAX_NUM_OF_EXTRACTS; i++) {
		data->params[param] |= u64_enc(0, 8, u_cfg[i].prot);
		data->params[param] |= u64_enc(8, 4, u_cfg[i].type);
		data->params[param] |= u64_enc(16, 8, u_cfg[i].size);
		data->params[param] |= u64_enc(24, 8, u_cfg[i].offset);
		data->params[param] |= u64_enc(32, 32, u_cfg[i].field);
		data->params[param] = cpu_to_le64(data->params[param]);
		param++;
		data->params[param] |= u64_enc(0, 8, u_cfg[i].hdr_index);
		data->params[param] |= u64_enc(8, 8, u_cfg[i].constant);
		data->params[param] |= u64_enc(16, 8, u_cfg[i].num_of_repeats);
		data->params[param] |= u64_enc(
		        24, 8, cfg->extracts[i].num_of_byte_masks);
		data->params[param] |= u64_enc(32, 4, cfg->extracts[i].type);
		data->params[param] = cpu_to_le64(data->params[param]);
		param++;
		for (j = 0; j < 4; j++) {
			data->params[param] |= u64_enc(
			        (offset), 8, cfg->extracts[i].masks[j].mask);
			data->params[param] |= u64_enc(
			        (offset + 8), 8,
			        cfg->extracts[i].masks[j].offset);
			offset += 16;
		}
		data->params[param] = cpu_to_le64(data->params[param]);
		param++;
	}

	return 0;
}

int dpni_create(struct fsl_mc_io *mc_io,
                const struct dpni_cfg *cfg,
                uint16_t *token)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CREATE,
	                                  MC_CMD_PRI_LOW,
	                                  0);
	DPNI_CMD_CREATE(cmd, cfg);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		*token = MC_CMD_HDR_READ_AUTHID(cmd.header);

	return err;
}

int dpni_open(struct fsl_mc_io *mc_io, int dpni_id, uint16_t *token)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_OPEN,
	                                  MC_CMD_PRI_LOW, 0);
	DPNI_CMD_OPEN(cmd, dpni_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		*token = MC_CMD_HDR_READ_AUTHID(cmd.header);

	return err;
}

int dpni_close(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLOSE,
	                                  MC_CMD_PRI_HIGH, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_destroy(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DESTROY,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_pools(struct fsl_mc_io *mc_io,
                   uint16_t token,
                   const struct dpni_pools_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_POOLS,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_SET_POOLS(cmd, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_irq(struct fsl_mc_io *mc_io,
                 uint16_t token,
                 uint8_t irq_index,
                 uint64_t irq_paddr,
                 uint32_t irq_val,
                 int user_irq_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IRQ,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_SET_IRQ(cmd, irq_index, irq_paddr, irq_val, user_irq_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_enable(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ENABLE,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_disable(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DISABLE,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_reset(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_RESET,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_attributes(struct fsl_mc_io *mc_io,
                        uint16_t token,
                        struct dpni_attr *attr)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_ATTR,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_ATTR(cmd, attr);

	return err;
}

int dpni_get_rx_buffer_layout(struct fsl_mc_io *mc_io,
                              uint16_t token,
                              struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_RX_BUFFER_LAYOUT,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_RX_BUFFER_LAYOUT(cmd, layout);

	return err;
}

int dpni_set_rx_buffer_layout(struct fsl_mc_io *mc_io,
                              uint16_t token,
                              const struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_BUFFER_LAYOUT,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_RX_BUFFER_LAYOUT(cmd, layout);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_tx_buffer_layout(struct fsl_mc_io *mc_io,
                              uint16_t token,
                              struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_BUFFER_LAYOUT,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_BUFFER_LAYOUT(cmd, layout);

	return err;
}

int dpni_set_tx_buffer_layout(struct fsl_mc_io *mc_io,
                              uint16_t token,
                              const struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_BUFFER_LAYOUT,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_TX_BUFFER_LAYOUT(cmd, layout);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
                                   uint16_t token,
                                   struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_CONF_BUFFER_LAYOUT,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_CONF_BUFFER_LAYOUT(cmd, layout);

	return err;
}

int dpni_set_tx_conf_buffer_layout(struct fsl_mc_io *mc_io,
                                   uint16_t token,
                                   const struct dpni_buffer_layout *layout)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_CONF_BUFFER_LAYOUT,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_TX_CONF_BUFFER_LAYOUT(cmd, layout);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_l3_chksum_validation(struct fsl_mc_io *mc_io,
                                  uint16_t token,
                                  int *en)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_L3_CHKSUM_VALIDATION,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_L3_CHKSUM_VALIDATION(cmd, *en);

	return err;
}

int dpni_set_l3_chksum_validation(struct fsl_mc_io *mc_io,
                                  uint16_t token,
                                  int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_L3_CHKSUM_VALIDATION,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_L3_CHKSUM_VALIDATION(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_l4_chksum_validation(struct fsl_mc_io *mc_io,
                                  uint16_t token,
                                  int *en)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_L4_CHKSUM_VALIDATION,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_L4_CHKSUM_VALIDATION(cmd, *en);
	}
	return err;
}

int dpni_set_l4_chksum_validation(struct fsl_mc_io *mc_io,
                                  uint16_t token,
                                  int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_L4_CHKSUM_VALIDATION,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_L4_CHKSUM_VALIDATION(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_qdid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *qdid)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_QDID,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_QDID(cmd, *qdid);

	return err;
}

int dpni_get_spid(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *spid)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_SPID,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_SPID(cmd, *spid);

	return err;
}

int dpni_get_tx_data_offset(struct fsl_mc_io *mc_io,
                            uint16_t token,
                            uint16_t *data_offset)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_DATA_OFFSET,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_TX_DATA_OFFSET(cmd, *data_offset);
	}
	return err;
}

int dpni_get_counter(struct fsl_mc_io *mc_io,
                     uint16_t token,
                     enum dpni_counter counter,
                     uint64_t *value)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_COUNTER,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_GET_COUNTER(cmd, counter);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_COUNTER(cmd, *value);

	return err;
}

int dpni_set_counter(struct fsl_mc_io *mc_io,
                     uint16_t token,
                     enum dpni_counter counter,
                     uint64_t value)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_COUNTER,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_COUNTER(cmd, counter, value);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_link_state(struct fsl_mc_io *mc_io, uint16_t token, int *up)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_LINK_STATE,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_LINK_STATE(cmd, *up);

	return err;
}

int dpni_set_mfl(struct fsl_mc_io *mc_io, uint16_t token, uint16_t mfl)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_MFL,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_SET_MFL(cmd, mfl);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_mfl(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *mfl)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_MFL,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_MFL(cmd, *mfl);

	return err;
}

int dpni_set_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t mtu)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_MTU,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_SET_MTU(cmd, mtu);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_mtu(struct fsl_mc_io *mc_io, uint16_t token, uint16_t *mtu)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_MTU,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_MTU(cmd, *mtu);
	return err;
}

int dpni_set_multicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_MCAST_PROMISC,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_MULTICAST_PROMISC(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_multicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int *en)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_MCAST_PROMISC,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_MULTICAST_PROMISC(cmd, *en);

	return err;
}

int dpni_set_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_UNICAST_PROMISC,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_UNICAST_PROMISC(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_unicast_promisc(struct fsl_mc_io *mc_io, uint16_t token, int *en)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_UNICAST_PROMISC,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_UNICAST_PROMISC(cmd, *en);

	return err;
}

int dpni_set_primary_mac_addr(struct fsl_mc_io *mc_io,
                              uint16_t token,
                              const uint8_t mac_addr[6])
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_PRIM_MAC,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_PRIMARY_MAC_ADDR(cmd, mac_addr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_primary_mac_addr(struct fsl_mc_io *mc_io,
                              uint16_t token,
                              uint8_t mac_addr[6])
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_PRIM_MAC,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_PRIMARY_MAC_ADDR(cmd, mac_addr);

	return err;
}

int dpni_add_mac_addr(struct fsl_mc_io *mc_io,
                      uint16_t token,
                      const uint8_t mac_addr[6])
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_MAC_ADDR,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_ADD_MAC_ADDR(cmd, mac_addr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_mac_addr(struct fsl_mc_io *mc_io,
                         uint16_t token,
                         const uint8_t mac_addr[6])
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_MAC_ADDR,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_REMOVE_MAC_ADDR(cmd, mac_addr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_clear_mac_table(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_MAC_TBL,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_add_vlan_id(struct fsl_mc_io *mc_io, uint16_t token, uint16_t vlan_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_VLAN_ID,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_ADD_VLAN_ID(cmd, vlan_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_vlan_id(struct fsl_mc_io *mc_io,
                        uint16_t token,
                        uint16_t vlan_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_VLAN_ID,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_REMOVE_VLAN_ID(cmd, vlan_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_clear_vlan_table(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_VLAN_TBL,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_tx_tc(struct fsl_mc_io *mc_io,
                   uint16_t token,
                   uint8_t tc_id,
                   const struct dpni_tx_tc_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_TC,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_SET_TX_TC(cmd, tc_id, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_rx_tc(struct fsl_mc_io *mc_io,
                   uint16_t token,
                   uint8_t tc_id,
                   const struct dpni_rx_tc_dist_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	struct extract_data ext_data = { { 0 } };
	uint64_t ext_paddr = virt_to_phys(&ext_data);
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_TC,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	err = build_extract_cfg_extention(cfg->dist_key_cfg, &ext_data);
	if (err)
		return err;
	DPNI_CMD_SET_RX_TC(cmd, tc_id, cfg, ext_paddr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_tx_flow(struct fsl_mc_io *mc_io,
                     uint16_t token,
                     uint16_t *flow_id,
                     const struct dpni_tx_flow_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_FLOW,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_TX_FLOW(cmd, *flow_id, cfg);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_SET_TX_FLOW(cmd, *flow_id);

	return err;
}

int dpni_get_tx_flow(struct fsl_mc_io *mc_io,
                     uint16_t token,
                     uint16_t flow_id,
                     struct dpni_tx_flow_cfg *cfg,
                     uint32_t *fqid)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_FLOW,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_GET_TX_FLOW(cmd, flow_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_FLOW(cmd, cfg, *fqid);

	return err;
}

int dpni_set_rx_flow(struct fsl_mc_io *mc_io,
                     uint16_t token,
                     uint8_t tc_id,
                     uint16_t flow_id,
                     const struct dpni_rx_flow_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_FLOW,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_RX_FLOW(cmd, tc_id, flow_id, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_rx_flow(struct fsl_mc_io *mc_io,
                     uint16_t token,
                     uint8_t tc_id,
                     uint16_t flow_id,
                     struct dpni_rx_flow_cfg *cfg,
                     uint32_t *fqid)
{
	struct mc_command cmd = { 0 };
	int err;
	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_RX_FLOW,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_GET_RX_FLOW(cmd, tc_id, flow_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_RX_FLOW(cmd, cfg, *fqid);

	return err;
}

int dpni_set_qos_table(struct fsl_mc_io *mc_io,
                       uint16_t token,
                       const struct dpni_qos_tbl_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	struct extract_data ext_data = { { 0 } };
	uint64_t ext_paddr = virt_to_phys(&ext_data);
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_QOS_TBL,
	                                  MC_CMD_PRI_LOW, token);
	err = build_extract_cfg_extention(cfg->qos_key_cfg, &ext_data);
	if (err)
		return err;
	DPNI_CMD_SET_QOS_TABLE(cmd, cfg, ext_paddr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_delete_qos_table(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_DELETE_QOS_TBL,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_add_qos_entry(struct fsl_mc_io *mc_io,
                       uint16_t token,
                       const struct dpni_rule_cfg *cfg,
                       uint8_t tc_id)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_QOS_ENT,
	                                  MC_CMD_PRI_LOW, token);
	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);

	DPNI_CMD_ADD_QOS_ENTRY(cmd, cfg, tc_id, key_paddr, mask_paddr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_qos_entry(struct fsl_mc_io *mc_io,
                          uint16_t token,
                          const struct dpni_rule_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_QOS_ENT,
	                                  MC_CMD_PRI_LOW, token);
	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);
	DPNI_CMD_REMOVE_QOS_ENTRY(cmd, cfg, key_paddr, mask_paddr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_clear_qos_table(struct fsl_mc_io *mc_io, uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_QOS_TBL,
	                                  MC_CMD_PRI_LOW, token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_add_fs_entry(struct fsl_mc_io *mc_io,
                      uint16_t token,
                      uint8_t tc_id,
                      const struct dpni_rule_cfg *cfg,
                      uint16_t flow_id)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_ADD_FS_ENT,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);
	DPNI_CMD_ADD_FS_ENTRY(cmd, tc_id, cfg, flow_id, key_paddr, mask_paddr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_remove_fs_entry(struct fsl_mc_io *mc_io,
                         uint16_t token,
                         uint8_t tc_id,
                         const struct dpni_rule_cfg *cfg)
{
	struct mc_command cmd = { 0 };
	uint64_t key_paddr, mask_paddr = 0;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_REMOVE_FS_ENT,
	                                  MC_CMD_PRI_LOW, token);
	key_paddr = virt_to_phys(cfg->key);
	if (cfg->mask)
		mask_paddr = virt_to_phys(cfg->mask);
	DPNI_CMD_REMOVE_FS_ENTRY(cmd, tc_id, cfg, key_paddr, mask_paddr);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_clear_fs_table(struct fsl_mc_io *mc_io, uint16_t token, uint8_t tc_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLR_FS_ENT,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_CLEAR_FS_ENTRIES(cmd, tc_id);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_irq(struct fsl_mc_io *mc_io,
                 uint16_t token,
                 uint8_t irq_index,
                 int *type,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val,
                 int *user_irq_id)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_GET_IRQ(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ(cmd, *type, *irq_paddr, *irq_val, *user_irq_id);

	return err;
}

int dpni_get_irq_enable(struct fsl_mc_io *mc_io,
                        uint16_t token,
                        uint8_t irq_index,
                        uint8_t *enable_state)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_ENABLE,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_GET_IRQ_ENABLE(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ_ENABLE(cmd, *enable_state);

	return err;
}

int dpni_set_irq_enable(struct fsl_mc_io *mc_io,
                        uint16_t token,
                        uint8_t irq_index,
                        uint8_t enable_state)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IRQ_ENABLE,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_IRQ_ENABLE(cmd, irq_index, enable_state);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);

}

int dpni_get_irq_mask(struct fsl_mc_io *mc_io,
                      uint16_t token,
                      uint8_t irq_index,
                      uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_MASK,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_GET_IRQ_MASK(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ_MASK(cmd, *mask);

	return err;
}

int dpni_set_irq_mask(struct fsl_mc_io *mc_io,
                      uint16_t token,
                      uint8_t irq_index,
                      uint32_t mask)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IRQ_MASK,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_IRQ_MASK(cmd, irq_index, mask);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_irq_status(struct fsl_mc_io *mc_io,
                        uint16_t token,
                        uint8_t irq_index,
                        uint32_t *status)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_IRQ_STATUS,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_GET_IRQ_STATUS(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_IRQ_STATUS(cmd, *status);

	return err;
}

int dpni_clear_irq_status(struct fsl_mc_io *mc_io,
                          uint16_t token,
                          uint8_t irq_index,
                          uint32_t status)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_CLEAR_IRQ_STATUS,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_CLEAR_IRQ_STATUS(cmd, irq_index, status);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_vlan_filters(struct fsl_mc_io *mc_io, uint16_t token, int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header( DPNI_CMDID_SET_VLAN_FILTERS,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_VLAN_FILTERS(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_ipr(struct fsl_mc_io *mc_io, uint16_t token, int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IPR,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_SET_IPR(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_ipf(struct fsl_mc_io *mc_io, uint16_t token, int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_IPF,
	                                  MC_CMD_PRI_LOW,
	                                  token);
	DPNI_CMD_SET_IPF(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_vlan_insertion(struct fsl_mc_io *mc_io, uint16_t token, int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_VLAN_INSERTION,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_VLAN_INSERTION(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_vlan_removal(struct fsl_mc_io *mc_io, uint16_t token, int en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_VLAN_INSERTION,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_VLAN_REMOVAL(cmd, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_set_rx_err_queue(struct fsl_mc_io *mc_io, uint16_t token,
	const struct dpni_rx_flow_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_RX_ERR_QUEUE,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_RX_ERR_QUEUE(cmd, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_rx_err_queue(struct fsl_mc_io *mc_io, uint16_t token,
	struct dpni_rx_flow_cfg *cfg, uint32_t *fqid)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_RX_ERR_QUEUE,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_RX_ERR_QUEUE(cmd, cfg, *fqid);

	return err;
}

int dpni_set_tx_conf_err_queue(struct fsl_mc_io *mc_io, uint16_t token,
	const struct dpni_rx_flow_cfg *cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_SET_TX_CONF_ERR_QUEUE,
	                                  MC_CMD_PRI_LOW, token);
	DPNI_CMD_SET_TX_CONF_ERR_QUEUE(cmd, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpni_get_tx_conf_err_queue(struct fsl_mc_io *mc_io, uint16_t token,
	struct dpni_rx_flow_cfg *cfg,
	uint32_t *fqid)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPNI_CMDID_GET_TX_CONF_ERR_QUEUE,
	                                  MC_CMD_PRI_LOW,
	                                  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (!err)
		DPNI_RSP_GET_TX_CONF_ERR_QUEUE(cmd, cfg, *fqid);

	return err;
}
