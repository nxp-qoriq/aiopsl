#include <fsl_dplib_sys.h>
#include <fsl_cmdif.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpni.h>
#include <fsl_dpni_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	cmd_data.params[_param] |= u64_enc(_offset, _width, _arg);

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	*(_arg) = (_type)u64_dec(cmd_data.params[_param], _offset, _width);

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	_arg = (_type)u64_dec(cmd_data.params[_param], _offset, _width);

int dpni_open(struct dpni *dpni, int dpni_id)
{
	return cmdif_open(&(dpni->cidesc), CMDIF_MOD_DPNI, (uint16_t)dpni_id);
}

int dpni_close(struct dpni *dpni)
{
	return cmdif_close(&(dpni->cidesc));
}

int dpni_init(struct dpni *dpni, const struct dpni_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_INIT(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_INIT, DPNI_CMDSZ_INIT,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpni_done(struct dpni *dpni)
{
	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_DONE, DPNI_CMDSZ_DONE,
				CMDIF_PRI_LOW, NULL);
}

int dpni_set_irq(struct dpni *dpni,
	uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_IRQ(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_IRQ,
				DPNI_CMDSZ_SET_IRQ, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_set_tx_pause_frames(struct dpni *dpni,
	uint8_t priority,
	uint16_t pause_time,
	uint16_t thresh_time)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_TX_PAUSE_FRAMES(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_TX_PAUSE_FRAMES,
				DPNI_CMDSZ_SET_TX_PAUSE_FRAMES, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_set_rx_ignore_pause_frames(struct dpni *dpni, int enable)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_RX_IGNORE_PAUSE_FRAMES(CMD_PREP);
	return cmdif_send(&(dpni->cidesc),
				DPNI_CMDID_SET_RX_IGNORE_PAUSE_FRAMES,
				DPNI_CMDSZ_SET_RX_IGNORE_PAUSE_FRAMES,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpni_attach(struct dpni *dpni, const struct dpni_attach_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_ATTACH(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_ATTACH,
				DPNI_CMDSZ_ATTACH, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_detach(struct dpni *dpni)
{
	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_DETACH,
				DPNI_CMDSZ_DETACH, CMDIF_PRI_LOW, NULL);
}

int dpni_attach_link(struct dpni *dpni, const struct dpni_attach_link_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_ATTACH_LINK(CMD_PREP);
	/* link_type */
	switch (cfg->link_type) {
	case DPNI_LINK_TYPE_DPMAC:
		cmd_data.params[0] |= u64_enc(16, 16, cfg->u.dpmac.id);
		break;

	case DPNI_LINK_TYPE_DPSW:
		cmd_data.params[0] |= u64_enc(8, 8, cfg->u.dpsw.if_id);
		cmd_data.params[0] |= u64_enc(16, 16, cfg->u.dpsw.id);
		break;

	case DPNI_LINK_TYPE_DPDMUX:
		cmd_data.params[0] |= u64_enc(8, 8, cfg->u.dpdmux.if_id);
		cmd_data.params[0] |= u64_enc(16, 16, cfg->u.dpdmux.id);
		break;

	case DPNI_LINK_TYPE_DPLAG:
		cmd_data.params[0] |= u64_enc(16, 16, cfg->u.dplag.id);
		break;

	case DPNI_LINK_TYPE_DPNI:
		cmd_data.params[0] |= u64_enc(16, 16, cfg->u.dpni.id);
		break;

	default:
		break;
	}
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_ATTACH_LINK,
				DPNI_CMDSZ_ATTACH_LINK, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_detach_link(struct dpni *dpni)
{
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_DETACH_LINK,
				DPNI_CMDSZ_DETACH_LINK, CMDIF_PRI_LOW, NULL);
}

int dpni_enable(struct dpni *dpni)
{

	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_ENABLE,
				DPNI_CMDSZ_ENABLE, CMDIF_PRI_LOW, NULL);
}

int dpni_disable(struct dpni *dpni)
{
	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_DISABLE,
				DPNI_CMDSZ_DISABLE, CMDIF_PRI_LOW, NULL);
}

int dpni_reset(struct dpni *dpni)
{
	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_RESET, DPNI_CMDSZ_RESET,
				CMDIF_PRI_LOW, NULL);
}

int dpni_get_attributes(struct dpni *dpni, struct dpni_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_ATTR,
				DPNI_CMDSZ_GET_ATTR, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_ATTR(RSP_READ_STRUCT);
	}
	return err;
}

int dpni_get_rx_buffer_layout(struct dpni *dpni,
	struct dpni_rx_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_RX_BUFFER_LAYOUT,
				DPNI_CMDSZ_GET_RX_BUFFER_LAYOUT, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_RX_BUFFER_LAYOUT(RSP_READ_STRUCT);
	}
	return err;
}

int dpni_set_rx_buffer_layout(struct dpni *dpni,
	struct dpni_rx_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_RX_BUFFER_LAYOUT(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_RX_BUFFER_LAYOUT,
				DPNI_CMDSZ_SET_RX_BUFFER_LAYOUT, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_qdid(struct dpni *dpni, uint16_t *qdid)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_QDID,
				DPNI_CMDSZ_GET_QDID, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_QDID(RSP_READ);
	}
	return err;
}

int dpni_get_tx_data_offset(struct dpni *dpni, uint16_t *data_offset)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_TX_DATA_OFFSET,
				DPNI_CMDSZ_GET_TX_DATA_OFFSET, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_TX_DATA_OFFSET(RSP_READ);
	}
	return err;
}

int dpni_get_counter(struct dpni *dpni,
	enum dpni_counter counter,
	uint64_t *value)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPNI_CMD_GET_COUNTER(CMD_PREP);

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_COUNTER,
				DPNI_CMDSZ_GET_COUNTER, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_COUNTER(RSP_READ);
	}
	return err;
}

int dpni_set_counter(struct dpni *dpni,
	enum dpni_counter counter,
	uint64_t value)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_COUNTER(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_COUNTER,
				DPNI_CMDSZ_GET_COUNTER, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_link_state(struct dpni *dpni, int *up)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* send command to mc*/
	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_LINK_STATE,
				DPNI_CMDSZ_GET_LINK_STATE, CMDIF_PRI_LOW,
				NULL);
	if (!err)
		DPNI_RSP_GET_LINK_STATE(RSP_READ);
	return err;
}

int dpni_set_mfl(struct dpni *dpni, uint16_t mfl)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_MFL(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_MFL,
				DPNI_CMDSZ_SET_MFL, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_mfl(struct dpni *dpni, uint16_t *mfl)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_MFL,
				DPNI_CMDSZ_GET_MFL, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_MFL(RSP_READ);
	return err;
}

int dpni_set_mtu(struct dpni *dpni, uint16_t mtu)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_MTU(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_MTU,
				DPNI_CMDSZ_SET_MTU, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_mtu(struct dpni *dpni, uint16_t *mtu)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_MTU,
				DPNI_CMDSZ_GET_MTU, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_MTU(RSP_READ);
	return err;
}

int dpni_set_multicast_promisc(struct dpni *dpni, int en)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_MULTICAST_PROMISC(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_MCAST_PROMISC,
				DPNI_CMDSZ_SET_MCAST_PROMISC, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_multicast_promisc(struct dpni *dpni, int *en)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_MCAST_PROMISC,
				DPNI_CMDSZ_GET_MCAST_PROMISC, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_MULTICAST_PROMISC(RSP_READ);
	return err;
}

int dpni_set_primary_mac_addr(struct dpni *dpni, const uint8_t addr[6])
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_PRIMARY_MAC_ADDR(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_PRIM_MAC,
				DPNI_CMDSZ_SET_PRIM_MAC, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_primary_mac_addr(struct dpni *dpni, uint8_t addr[6])
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_PRIM_MAC,
				DPNI_CMDSZ_GET_PRIM_MAC, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_PRIMARY_MAC_ADDR(RSP_READ_STRUCT);
	/* TODO - check */
	return err;
}

int dpni_add_mac_addr(struct dpni *dpni, const uint8_t addr[6])
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_ADD_MAC_ADDR(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_ADD_MAC_ADDR,
				DPNI_CMDSZ_ADD_MAC_ADDR, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_remove_mac_addr(struct dpni *dpni, const uint8_t addr[6])
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_REMOVE_MAC_ADDR(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_REMOVE_MAC_ADDR,
				DPNI_CMDSZ_REMOVE_MAC_ADDR, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_clear_mac_table(struct dpni *dpni)
{

	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_CLR_MAC_TBL,
				DPNI_CMDSZ_CLR_MAC_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_add_vlan_id(struct dpni *dpni, uint16_t vlan_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_ADD_VLAN_ID(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_ADD_VLAN_ID,
				DPNI_CMDSZ_ADD_VLAN_ID, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_remove_vlan_id(struct dpni *dpni, uint16_t vlan_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_REMOVE_VLAN_ID(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_REMOVE_VLAN_ID,
				DPNI_CMDSZ_REMOVE_VLAN_ID, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_clear_vlan_table(struct dpni *dpni)
{

	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_CLR_VLAN_TBL,
				DPNI_CMDSZ_CLR_VLAN_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_set_tx_tc(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_tx_tc_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_SET_TX_TC(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_TX_TC,
				DPNI_CMDSZ_SET_TX_TC, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_set_rx_tc(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_rx_tc_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_SET_RX_TC(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_RX_TC,
				DPNI_CMDSZ_SET_RX_TC, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_set_tx_flow(struct dpni *dpni,
	uint16_t *flow_id,
	const struct dpni_tx_flow_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_SET_TX_FLOW(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_TX_FLOW,
				DPNI_CMDSZ_SET_TX_FLOW, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_tx_flow(struct dpni *dpni,
	uint16_t flow_id,
	struct dpni_tx_flow_cfg *cfg,
	uint32_t *fqid)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	/* prepare command */
	DPNI_CMD_GET_TX_FLOW(CMD_PREP);

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_GET_TX_FLOW,
				DPNI_CMDSZ_GET_TX_FLOW, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_TX_FLOW(RSP_READ_STRUCT);
	return err;
}

int dpni_set_rx_flow(struct dpni *dpni,
	uint8_t tc_id,
	uint16_t flow_id,
	const struct dpni_rx_flow_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_SET_RX_FLOW(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_RX_FLOW,
				DPNI_CMDSZ_SET_RX_FLOW, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_get_rx_flow(struct dpni *dpni,
	uint8_t tc_id,
	uint16_t flow_id,
	struct dpni_rx_flow_cfg *cfg,
	uint32_t *fqid)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	/* prepare command */
	DPNI_CMD_GET_RX_FLOW(CMD_PREP);

	err = cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_RX_FLOW,
				DPNI_CMDSZ_SET_RX_FLOW, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_RX_FLOW(RSP_READ_STRUCT);
	return err;
}

int dpni_set_qos_table(struct dpni *dpni, const struct dpni_qos_tbl_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_QOS_TABLE(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_QOS_TBL,
				DPNI_CMDSZ_SET_QOS_TBL, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_delete_qos_table(struct dpni *dpni)
{

	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_DELETE_QOS_TBL,
				DPNI_CMDSZ_DELETE_QOS_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_add_qos_entry(struct dpni *dpni,
	const struct dpni_key_cfg *cfg,
	uint8_t tc_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_ADD_QOS_ENTRY(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_ADD_QOS_ENT,
				DPNI_CMDSZ_ADD_QOS_ENT, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_remove_qos_entry(struct dpni *dpni, const struct dpni_key_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_REMOVE_QOS_ENTRY(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_REMOVE_QOS_ENT,
				DPNI_CMDSZ_REMOVE_QOS_ENT, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_clear_qos_table(struct dpni *dpni)
{

	/* send command to mc*/
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_CLR_QOS_TBL,
				DPNI_CMDSZ_CLR_QOS_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_set_dist(struct dpni *dpni,
	const struct dpni_dist_cfg dist[DPNI_MAX_TC])
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_DIST(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_DIST,
				DPNI_CMDSZ_SET_DIST, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_set_fs_table(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_fs_tbl_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_FS_TABLE(CMD_PREP);

	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_SET_FS_TBL,
				DPNI_CMDSZ_SET_FS_TBL, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_delete_fs_table(struct dpni *dpni, uint8_t tc_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_DELETE_FS_TABLE(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_DELETE_FS_TBL,
				DPNI_CMDSZ_DELETE_FS_TBL, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_add_fs_entry(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_key_cfg *cfg,
	uint16_t flow_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_ADD_FS_ENTRY(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_ADD_FS_ENT,
				DPNI_CMDSZ_ADD_FS_ENT, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_remove_fs_entry(struct dpni *dpni,
	uint8_t tc_id,
	const struct dpni_key_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_REMOVE_FS_ENTRY(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_REMOVE_FS_ENT,
				DPNI_CMDSZ_REMOVE_FS_ENT, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}

int dpni_clear_fs_table(struct dpni *dpni, uint8_t tc_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_CLEAR_FS_TABLE(CMD_PREP);
	return cmdif_send(&(dpni->cidesc), DPNI_CMDID_CLR_FS_TBL,
				DPNI_CMDSZ_CLR_FS_TBL, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
}
