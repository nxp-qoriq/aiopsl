/**************************************************************************//*
 @File          dpsw.c

 @Description   L2 switch FLib implementation file

 @Cautions      None.
 *//***************************************************************************/
#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpsw.h>
#include <fsl_dpsw_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

int dpsw_create(struct dpsw *dpsw, const struct dpsw_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_CREATE(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpsw->auth = 0;

	/* send command to mc*/
	return dplib_send(dpsw->regs, &(dpsw->auth), MC_DPSW_CMDID_CREATE,
				DPSW_CMDSZ_CREATE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_open(struct dpsw *dpsw, int dpsw_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_OPEN(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpsw->auth = 0;

	/* send command to mc*/
	return dplib_send(dpsw->regs, &(dpsw->auth), MC_DPSW_CMDID_OPEN,
				MC_CMD_OPEN_SIZE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_close(struct dpsw *dpsw)
{
	return dplib_send(dpsw->regs, &(dpsw->auth), MC_CMDID_CLOSE,
				MC_CMD_CLOSE_SIZE, MC_CMD_PRI_HIGH, NULL);
}

int dpsw_get_attributes(struct dpsw *dpsw, struct dpsw_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_GET_ATTR,
			 DPSW_CMDSZ_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPSW_RSP_GET_ATTR(RSP_READ_STRUCT);
	}
	return err;
}

int dpsw_destroy(struct dpsw *dpsw)
{
	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_DESTROY,
				DPSW_CMDSZ_DESTROY, MC_CMD_PRI_LOW, NULL);
}

int dpsw_enable(struct dpsw *dpsw)
{
	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_ENABLE,
				DPSW_CMDSZ_ENABLE, MC_CMD_PRI_LOW, NULL);
}

int dpsw_disable(struct dpsw *dpsw)
{
	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_DISABLE,
				DPSW_CMDSZ_DISABLE, MC_CMD_PRI_LOW, NULL);
}

int dpsw_reset(struct dpsw *dpsw)
{
	/* send command to mc*/
	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_RESET,
				DPSW_CMDSZ_RESET, MC_CMD_PRI_LOW, NULL);
}

int dpsw_set_policer(struct dpsw *dpsw, const struct dpsw_policer_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_SET_POLICER(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_SET_POLICER,
				DPSW_CMDSZ_SET_POLICER, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_set_buffer_depletion(struct dpsw *dpsw,
			      const struct dpsw_buffer_depletion_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_SET_BUFFER_DEPLETION(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_SET_BUFFER_DEPLETION,
				DPSW_CMDSZ_SET_BUFFER_DEPLETION,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_set_reflection_if(struct dpsw *dpsw, uint16_t if_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_SET_REFLECTION_IF(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_SET_REFLECTION_IF,
				DPSW_CMDSZ_SET_REFLECTION_IF, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_set_ptp_v2(struct dpsw *dpsw, const struct dpsw_ptp_v2_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_SET_PTP_V2(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_SET_PTP_V2,
				DPSW_CMDSZ_SET_PTP_V2, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_set_tci(struct dpsw *dpsw,
		    uint16_t if_id,
	const struct dpsw_tci_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_TCI(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_SET_TCI,
				DPSW_CMDSZ_IF_SET_TCI, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_set_stp(struct dpsw *dpsw,
		    uint16_t if_id,
	const struct dpsw_stp_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_STP_STATE(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_SET_STP_STATE,
				DPSW_CMDSZ_IF_SET_STP_STATE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_set_accepted_frames(struct dpsw *dpsw,
				uint16_t if_id,
	const struct dpsw_accepted_frames_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_ACCEPTED_FRAMES(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_SET_ACCEPTED_FRAMES,
				DPSW_CMDSZ_IF_SET_ACCEPTED_FRAMES,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_if_set_accept_all_vlan(struct dpsw *dpsw,
				uint16_t if_id,
	int accept_all)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_ACCEPT_ALL_VLAN(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_SET_IF_ACCEPT_ALL_VLAN,
				DPSW_CMDSZ_SET_IF_ACCEPT_ALL_VLAN,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_if_get_counter(struct dpsw *dpsw,
			uint16_t if_id,
	enum dpsw_counter type,
	uint64_t *counter)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPSW_CMD_IF_GET_COUNTER(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_GET_COUNTER,
			 DPSW_CMDSZ_IF_GET_COUNTER, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPSW_RSP_IF_GET_COUNTER(RSP_READ);

	return err;
}

int dpsw_if_set_counter(struct dpsw *dpsw,
			uint16_t if_id,
	enum dpsw_counter type,
	uint64_t counter)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_COUNTER(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_SET_COUNTER,
				DPSW_CMDSZ_IF_SET_COUNTER, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_tc_set_map(struct dpsw *dpsw,
		       uint16_t if_id,
	const struct dpsw_tc_map_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_TC_SET_MAP(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_TC_SET_MAP,
				DPSW_CMDSZ_IF_TC_SET_MAP, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_add_reflection(struct dpsw *dpsw,
			   uint16_t if_id,
	const struct dpsw_reflection_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_ADD_REFLECTION(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_ADD_REFLECTION,
				DPSW_CMDSZ_IF_ADD_REFLECTION, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_remove_reflection(struct dpsw *dpsw,
			      uint16_t if_id,
	const struct dpsw_reflection_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_REMOVE_REFLECTION(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_REMOVE_REFLECTION,
				DPSW_CMDSZ_IF_REMOVE_REFLECTION,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_if_tc_set_metering_marking(struct dpsw *dpsw,
				    uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_metering_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_TC_SET_METERING_MARKING(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_TC_SET_METERING_MARKING,
				DPSW_CMDSZ_IF_TC_SET_METERING_MARKING,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_if_add_custom_tpid(struct dpsw *dpsw,
			    uint16_t if_id,
	const struct dpsw_custom_tpid_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_ADD_CUSTOM_TPID(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_ADD_CUSTOM_TPID,
				DPSW_CMDSZ_IF_ADD_CUSTOM_TPID, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_set_transmit_rate(struct dpsw *dpsw,
			      uint16_t if_id,
	const struct dpsw_transmit_rate_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_TRANSMIT_RATE(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDSZ_IF_SET_TRANSMIT_RATE,
				DPSW_CMDID_IF_SET_TRANSMIT_RATE,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_if_tc_set_bandwidth(struct dpsw *dpsw,
			     uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_bandwidth_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_TC_SET_BANDWIDTH(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_TC_SET_BW,
				DPSW_CMDSZ_IF_TC_SET_BW, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_enable(struct dpsw *dpsw, uint16_t if_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_ENABLE(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_ENABLE,
				DPSW_CMDSZ_IF_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_disable(struct dpsw *dpsw, uint16_t if_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_DISABLE(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_DISABLE,
				DPSW_CMDSZ_IF_DISABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_tc_set_queue_congestion(struct dpsw *dpsw,
				    uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_queue_congestion_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_TC_SET_Q_CONGESTION(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_TC_SET_Q_CONGESTION,
				DPSW_CMDSZ_IF_TC_SET_Q_CONGESTION,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_if_tc_set_pfc(struct dpsw *dpsw,
		       uint16_t if_id,
	uint8_t tc_id,
	struct dpsw_pfc_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_TC_SET_PFC(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_TC_SET_PFC,
				DPSW_CMDSZ_IF_TC_SET_PFC, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_tc_set_cn(struct dpsw *dpsw,
		      uint16_t if_id,
	uint8_t tc_id,
	const struct dpsw_cn_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_TC_SET_CN(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_TC_SET_CN,
				DPSW_CMDSZ_IF_TC_SET_CN, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_get_attributes(struct dpsw *dpsw,
			   uint16_t if_id,
	struct dpsw_if_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPSW_CMD_IF_GET_ATTR(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_GET_ATTR,
			 DPSW_CMDSZ_IF_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPSW_RSP_IF_GET_ATTR(RSP_READ_STRUCT);

	return err;
}

int dpsw_if_set_macsec(struct dpsw *dpsw,
		       uint16_t if_id,
	const struct dpsw_macsec_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_MACSEC(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_IF_SET_MACSEC,
				DPSW_CMDSZ_IF_SET_MACSEC, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_if_set_max_frame_length(struct dpsw *dpsw,
				 uint16_t if_id,
	uint16_t frame_length)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_IF_SET_MAX_FRAME_LENGTH(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_IF_SET_MAX_FRAME_LENGTH,
				DPSW_CMDSZ_IF_SET_MAX_FRAME_LENGTH,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_vlan_add(struct dpsw *dpsw,
		  uint16_t vlan_id,
	const struct dpsw_vlan_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_VLAN_ADD(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_VLAN_ADD,
				DPSW_CMDSZ_VLAN_ADD, MC_CMD_PRI_LOW, &cmd_data);
}

static void build_if_id_bitmap(const uint16_t *if_id,
			       const uint16_t num_ifs,
	struct mc_cmd_data *cmd_data,
	int start_param)
{
	int i;

	for (i = 0; (i < num_ifs) && (i < DPSW_MAX_IF); i++)
		cmd_data->params[start_param + (if_id[i] / 64)] |= u64_enc(
			(if_id[i] % 64), 1, 1);
}

int dpsw_vlan_add_if(struct dpsw *dpsw,
		     uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 1);

	DPSW_CMD_VLAN_ADD_IF(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_VLAN_ADD_IF,
				DPSW_CMDSZ_VLAN_ADD_IF, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_vlan_add_if_untagged(struct dpsw *dpsw,
			      uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 1);

	DPSW_CMD_VLAN_ADD_IF_UNTAGGED(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_VLAN_ADD_IF_UNTAGGED,
				DPSW_CMDSZ_VLAN_ADD_IF_UNTAGGED,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_vlan_add_if_flooding(struct dpsw *dpsw,
			      uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 1);

	DPSW_CMD_VLAN_ADD_IF_FLOODING(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_VLAN_ADD_IF_FLOODING,
				DPSW_CMDSZ_VLAN_ADD_IF_FLOODING,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_vlan_remove_if(struct dpsw *dpsw,
			uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 1);

	DPSW_CMD_VLAN_REMOVE_IF(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_VLAN_REMOVE_IF,
				DPSW_CMDSZ_VLAN_REMOVE_IF, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_vlan_remove_if_untagged(struct dpsw *dpsw,
				 uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 1);

	DPSW_CMD_VLAN_REMOVE_IF_UNTAGGED(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_VLAN_REMOVE_IF_UNTAGGED,
				DPSW_CMDSZ_VLAN_REMOVE_IF_UNTAGGED,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_vlan_remove_if_flooding(struct dpsw *dpsw,
				 uint16_t vlan_id,
	const struct dpsw_vlan_if_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 1);

	DPSW_CMD_VLAN_REMOVE_IF_FLOODING(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_VLAN_REMOVE_IF_FLOODING,
				DPSW_CMDSZ_VLAN_REMOVE_IF_FLOODING,
				MC_CMD_PRI_LOW, &cmd_data);
}
int dpsw_vlan_remove(struct dpsw *dpsw, uint16_t vlan_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_VLAN_REMOVE(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_VLAN_REMOVE,
				DPSW_CMDSZ_VLAN_REMOVE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_fdb_add(struct dpsw *dpsw,
		 uint16_t *fdb_id,
	const struct dpsw_fdb_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPSW_CMD_FDB_ADD(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_FDB_ADD,
			 DPSW_CMDSZ_FDB_ADD, MC_CMD_PRI_LOW, &cmd_data);
	if (!err)
		DPSW_RSP_FDB_ADD(RSP_READ);

	return err;
}

int dpsw_fdb_remove(struct dpsw *dpsw, uint16_t fdb_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_FDB_REMOVE(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_FDB_REMOVE,
				DPSW_CMDSZ_FDB_REMOVE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_fdb_add_unicast(struct dpsw *dpsw,
			 uint16_t fdb_id,
	const struct dpsw_fdb_unicast_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_FDB_ADD_UNICAST(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_FDB_ADD_UNICAST,
				DPSW_CMDSZ_FDB_ADD_UNICAST, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_fdb_remove_unicast(struct dpsw *dpsw,
			    uint16_t fdb_id,
	const struct dpsw_fdb_unicast_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_FDB_REMOVE_UNICAST(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_FDB_REMOVE_UNICAST,
				DPSW_CMDSZ_FDB_REMOVE_UNICAST, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_fdb_add_multicast(struct dpsw *dpsw,
			   uint16_t fdb_id,
	const struct dpsw_fdb_multicast_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 2);

	DPSW_CMD_FDB_ADD_MULTICAST(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_FDB_ADD_MULTICAST,
				DPSW_CMDSZ_FDB_ADD_MULTICAST, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_fdb_remove_multicast(struct dpsw *dpsw,
			      uint16_t fdb_id,
	const struct dpsw_fdb_multicast_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	build_if_id_bitmap(cfg->if_id, cfg->num_ifs, &cmd_data, 2);

	DPSW_CMD_FDB_REMOVE_MULTICAST(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_FDB_REMOVE_MULTICAST,
				DPSW_CMDSZ_FDB_REMOVE_MULTICAST,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_fdb_set_learning_mode(struct dpsw *dpsw,
			       uint16_t fdb_id,
	enum dpsw_fdb_learning_mode mode)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_FDB_SET_LEARNING_MODE(CMD_PREP);

	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_FDB_SET_LEARNING_MODE,
				DPSW_CMDSZ_FDB_SET_LEARNING_MODE,
				MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_fdb_get_attributes(struct dpsw *dpsw,
			    uint16_t fdb_id,
	struct dpsw_fdb_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPSW_CMD_FDB_GET_ATTR(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_FDB_GET_ATTR,
			 DPSW_CMDSZ_FDB_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPSW_RSP_FDB_GET_ATTR(RSP_READ_STRUCT);

	return err;
}

int dpsw_get_irq(struct dpsw *dpsw,
		 uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPSW_CMD_GET_IRQ(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_GET_IRQ,
			 DPSW_CMDSZ_GET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
	if (!err)
		DPSW_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpsw_set_irq(struct dpsw *dpsw,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_SET_IRQ,
				DPSW_CMDSZ_SET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
}

int dpsw_get_irq_enable(struct dpsw *dpsw,
			uint8_t irq_index,
	uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPSW_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_GET_IRQ_ENABLE,
			 DPSW_CMDSZ_GET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPSW_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dpsw_set_irq_enable(struct dpsw *dpsw,
			uint8_t irq_index,
	uint8_t enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_SET_IRQ_ENABLE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_SET_IRQ_ENABLE,
				DPSW_CMDSZ_SET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_get_irq_mask(struct dpsw *dpsw, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPSW_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_GET_IRQ_MASK,
			 DPSW_CMDSZ_GET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPSW_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpsw_set_irq_mask(struct dpsw *dpsw, uint8_t irq_index, uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_SET_IRQ_MASK,
				DPSW_CMDSZ_SET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpsw_get_irq_status(struct dpsw *dpsw, uint8_t irq_index, uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPSW_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(dpsw->regs, &(dpsw->auth), DPSW_CMDID_GET_IRQ_STATUS,
			 DPSW_CMDSZ_GET_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPSW_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpsw_clear_irq_status(struct dpsw *dpsw,
			  uint8_t irq_index,
	uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPSW_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpsw->regs, &(dpsw->auth),
				DPSW_CMDID_CLEAR_IRQ_STATUS,
				DPSW_CMDSZ_CLEAR_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
}

