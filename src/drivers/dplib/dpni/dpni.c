#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpni.h>
#include <fsl_dpni_cmd.h>

#define EXT_PREP(_param, _offset, _width, _type, _arg) \
	(ext_data->params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

static int build_extract_cfg_extention(struct dpkg_profile_cfg *extract_cfg, struct extract_data *ext_data) 
{
	int i;
	struct {
		enum net_prot prot;
		enum dpkg_extract_from_hdr_type type;
		uint32_t field;
		uint8_t size;
		uint8_t offset;
		uint8_t hdr_index;
		enum dpkg_extract_from_context_type src;
		uint8_t constant;
		uint8_t num_of_repeats;
		uint8_t num_of_byte_masks;
	}union_cfg[DPKG_MAX_NUM_OF_EXTRACTS];
	for (i = 0; i < DPKG_MAX_NUM_OF_EXTRACTS; i++){
		switch(extract_cfg->extracts[i].type){
		case DPKG_EXTRACT_FROM_HDR:
			union_cfg[i].prot = extract_cfg->extracts[i].extract.from_hdr.prot;
			union_cfg[i].type = extract_cfg->extracts[i].extract.from_hdr.type;
			union_cfg[i].field = extract_cfg->extracts[i].extract.from_hdr.field;
			union_cfg[i].size = extract_cfg->extracts[i].extract.from_hdr.size;
			union_cfg[i].offset = extract_cfg->extracts[i].extract.from_hdr.offset;
			union_cfg[i].hdr_index = extract_cfg->extracts[i].extract.from_hdr.hdr_index;
			break;
		case DPKG_EXTRACT_FROM_DATA:
			union_cfg[i].size = extract_cfg->extracts[i].extract.from_data.size;
			union_cfg[i].offset = extract_cfg->extracts[i].extract.from_data.offset;
			break;
		case DPKG_EXTRACT_FROM_CONTEXT:
			union_cfg[i].src = extract_cfg->extracts[i].extract.from_context.src;
			union_cfg[i].size = extract_cfg->extracts[i].extract.from_context.size;
			union_cfg[i].offset = extract_cfg->extracts[i].extract.from_context.offset;
			break;
		case DPKG_EXTRACT_CONSTANT:
			union_cfg[i].constant = extract_cfg->extracts[i].extract.constant.constant;
			union_cfg[i].num_of_repeats = extract_cfg->extracts[i].extract.constant.num_of_repeats;
			break;
		default :
			pr_err("invalid extract type %d\n", (int)extract_cfg->extracts[i].type);
			return -EINVAL;
		}
	}
	DPNI_EXT_EXTRACT_CFG(EXT_PREP);
	return 0;
}

int dpni_create(struct dpni *dpni, const struct dpni_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPNI_CMD_CREATE(CMD_PREP);

	/* send command to mc*/

	/* clear 'dev' - later it will store the Authentication ID */
	dpni->auth = 0;

	/* ----------TBD lock------------------ */
	err = dplib_send(dpni->regs, 0, MC_DPNI_CMDID_CREATE, DPNI_CMDSZ_CREATE,
	                 CMDIF_PRI_LOW, &cmd_data);
	if (!err)
		dpni->auth = (int)mc_cmd_read_auth_id(dpni->regs);
	/* ----------TBD unlock------------------ */

	return err;
}

int dpni_open(struct dpni *dpni, int dpni_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPNI_CMD_OPEN(CMD_PREP);

	/* send command to mc*/
	dpni->auth = 0;

	/* ----------TBD lock------------------ */
	err = dplib_send(dpni->regs, 0, MC_DPNI_CMDID_OPEN, MC_CMD_OPEN_SIZE,
	                 CMDIF_PRI_LOW, &cmd_data);
	if (!err)
		dpni->auth = (int)mc_cmd_read_auth_id(dpni->regs);
	/* ----------TBD unlock------------------ */

	return err;
}

int dpni_close(struct dpni *dpni)
{
	return dplib_send(dpni->regs, 0, MC_CMDID_CLOSE, MC_CMD_CLOSE_SIZE,
		                 CMDIF_PRI_LOW, NULL);
}

int dpni_destroy(struct dpni *dpni)
{
	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_DESTROY,
			  DPNI_CMDSZ_DESTROY, CMDIF_PRI_LOW, NULL);
}

int dpni_set_pools(struct dpni *dpni,
		 const struct dpni_pools_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_POOLS(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_POOLS,
			  DPNI_CMDSZ_SET_POOLS, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_set_irq(struct dpni *dpni,
		 uint8_t irq_index,
		 uint64_t irq_paddr,
		 uint32_t irq_val,
		 int irq_virt_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_IRQ(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_IRQ,
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
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_TX_PAUSE_FRAMES,
			  DPNI_CMDSZ_SET_TX_PAUSE_FRAMES, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_set_rx_ignore_pause_frames(struct dpni *dpni, int enable)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_RX_IGNORE_PAUSE_FRAMES(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth,
			  DPNI_CMDID_SET_RX_IGNORE_PAUSE_FRAMES,
			  DPNI_CMDSZ_SET_RX_IGNORE_PAUSE_FRAMES, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_attach(struct dpni *dpni, const struct dpni_attach_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_ATTACH(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_ATTACH, DPNI_CMDSZ_ATTACH,
			  CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpni_detach(struct dpni *dpni)
{
	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_DETACH, DPNI_CMDSZ_DETACH,
			  CMDIF_PRI_LOW, NULL);
}

int dpni_enable(struct dpni *dpni)
{
	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_ENABLE, DPNI_CMDSZ_ENABLE,
			  CMDIF_PRI_LOW, NULL);
}

int dpni_disable(struct dpni *dpni)
{
	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_DISABLE,
			  DPNI_CMDSZ_DISABLE, CMDIF_PRI_LOW, NULL);
}

int dpni_reset(struct dpni *dpni)
{
	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_RESET, DPNI_CMDSZ_RESET,
			  CMDIF_PRI_LOW, NULL);
}

int dpni_get_attributes(struct dpni *dpni, struct dpni_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_ATTR,
			 DPNI_CMDSZ_GET_ATTR, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_ATTR(RSP_READ_STRUCT);
	}
	return err;
}

int dpni_get_rx_buffer_layout(struct dpni *dpni,
			      struct dpni_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_RX_BUFFER_LAYOUT,
			 DPNI_CMDSZ_GET_RX_BUFFER_LAYOUT, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_RX_BUFFER_LAYOUT(RSP_READ_STRUCT);
	}
	return err;
}

int dpni_set_rx_buffer_layout(struct dpni *dpni,
			      struct dpni_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_RX_BUFFER_LAYOUT(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_RX_BUFFER_LAYOUT,
			  DPNI_CMDSZ_SET_RX_BUFFER_LAYOUT, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_tx_buffer_layout(struct dpni *dpni,
			      struct dpni_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_TX_BUFFER_LAYOUT,
			 DPNI_CMDSZ_GET_TX_BUFFER_LAYOUT, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_TX_BUFFER_LAYOUT(RSP_READ_STRUCT);
	}
	return err;
}

int dpni_set_tx_buffer_layout(struct dpni *dpni,
			      struct dpni_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_TX_BUFFER_LAYOUT(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_TX_BUFFER_LAYOUT,
			  DPNI_CMDSZ_SET_TX_BUFFER_LAYOUT, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_tx_conf_buffer_layout(struct dpni *dpni,
			      struct dpni_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_TX_CONF_BUFFER_LAYOUT,
			 DPNI_CMDSZ_GET_TX_CONF_BUFFER_LAYOUT, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_TX_CONF_BUFFER_LAYOUT(RSP_READ_STRUCT);
	}
	return err;
}

int dpni_set_tx_conf_buffer_layout(struct dpni *dpni,
			      struct dpni_buffer_layout *layout)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_TX_CONF_BUFFER_LAYOUT(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_TX_CONF_BUFFER_LAYOUT,
			  DPNI_CMDSZ_SET_TX_CONF_BUFFER_LAYOUT, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_l3_CHKSUM_validation(struct dpni *dpni,
                                 int *en)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_L3_CHKSUM_VALIDATION,
	                 DPNI_CMDSZ_GET_L3_CHKSUM_VALIDATION, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_L3_CHKSUM_VALIDATION(RSP_READ);
	}
	return err;
}

int dpni_set_l3_CHKSUM_validation(struct dpni *dpni,
                                   int en)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_L3_CHKSUM_VALIDATION(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_L3_CHKSUM_VALIDATION,
		                 DPNI_CMDSZ_SET_L3_CHKSUM_VALIDATION, CMDIF_PRI_LOW,
				 (uint8_t *)&cmd_data);
}

int dpni_get_l4_CHKSUM_validation(struct dpni *dpni,
                                 int *en)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_L4_CHKSUM_VALIDATION,
	                 DPNI_CMDSZ_GET_L4_CHKSUM_VALIDATION, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPNI_RSP_GET_L4_CHKSUM_VALIDATION(RSP_READ);
	}
	return err;
}

int dpni_set_l4_CHKSUM_validation(struct dpni *dpni,
                                   int en)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_L4_CHKSUM_VALIDATION(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_L4_CHKSUM_VALIDATION,
		                 DPNI_CMDSZ_SET_L4_CHKSUM_VALIDATION, CMDIF_PRI_LOW,
				 (uint8_t *)&cmd_data);
}

int dpni_get_qdid(struct dpni *dpni, uint16_t *qdid)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_QDID,
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

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_TX_DATA_OFFSET,
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

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_COUNTER,
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

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_COUNTER,
			  DPNI_CMDSZ_GET_COUNTER, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_link_state(struct dpni *dpni, int *up)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* send command to mc*/
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_LINK_STATE,
			 DPNI_CMDSZ_GET_LINK_STATE, CMDIF_PRI_LOW, NULL);
	if (!err)
		DPNI_RSP_GET_LINK_STATE(RSP_READ);
	return err;
}

int dpni_set_mfl(struct dpni *dpni, uint16_t mfl)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_MFL(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_MFL,
			  DPNI_CMDSZ_SET_MFL, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_mfl(struct dpni *dpni, uint16_t *mfl)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_MFL,
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
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_MTU,
			  DPNI_CMDSZ_SET_MTU, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_mtu(struct dpni *dpni, uint16_t *mtu)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_MTU,
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

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_MCAST_PROMISC,
			  DPNI_CMDSZ_SET_MCAST_PROMISC, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_multicast_promisc(struct dpni *dpni, int *en)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_MCAST_PROMISC,
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
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_PRIM_MAC,
			  DPNI_CMDSZ_SET_PRIM_MAC, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_primary_mac_addr(struct dpni *dpni, uint8_t addr[6])
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_PRIM_MAC,
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
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_ADD_MAC_ADDR,
			  DPNI_CMDSZ_ADD_MAC_ADDR, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_remove_mac_addr(struct dpni *dpni, const uint8_t addr[6])
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_REMOVE_MAC_ADDR(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_REMOVE_MAC_ADDR,
			  DPNI_CMDSZ_REMOVE_MAC_ADDR, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_clear_mac_table(struct dpni *dpni)
{
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_CLR_MAC_TBL,
			  DPNI_CMDSZ_CLR_MAC_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_add_vlan_id(struct dpni *dpni, uint16_t vlan_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_ADD_VLAN_ID(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_ADD_VLAN_ID,
			  DPNI_CMDSZ_ADD_VLAN_ID, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_remove_vlan_id(struct dpni *dpni, uint16_t vlan_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_REMOVE_VLAN_ID(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_REMOVE_VLAN_ID,
			  DPNI_CMDSZ_REMOVE_VLAN_ID, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_clear_vlan_table(struct dpni *dpni)
{
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_CLR_VLAN_TBL,
			  DPNI_CMDSZ_CLR_VLAN_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_set_tx_tc(struct dpni *dpni,
		   uint8_t tc_id,
		   const struct dpni_tx_tc_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_SET_TX_TC(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_TX_TC,
			  DPNI_CMDSZ_SET_TX_TC, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_set_rx_tc(struct dpni *dpni,
		   uint8_t tc_id,
		   const struct dpni_rx_tc_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	struct extract_data ext_data = { { 0 } };
	uint64_t ext_paddr = virt_to_phys(&ext_data);
	int err;
	
	/* prepare command */
	err = build_extract_cfg_extention(cfg->extract_cfg, &ext_data);
	if (err)
		return err;
	DPNI_CMD_SET_RX_TC(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_RX_TC,
			  DPNI_CMDSZ_SET_RX_TC, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_set_tx_flow(struct dpni *dpni,
		     uint16_t *flow_id,
		     const struct dpni_tx_flow_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	/* prepare command */
	DPNI_CMD_SET_TX_FLOW(CMD_PREP);

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_TX_FLOW,
			 DPNI_CMDSZ_SET_TX_FLOW, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_SET_TX_FLOW(RSP_READ_STRUCT);
	return err;
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

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_TX_FLOW,
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

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_RX_FLOW,
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

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_RX_FLOW,
			 DPNI_CMDSZ_GET_RX_FLOW, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_RX_FLOW(RSP_READ_STRUCT);
	return err;
}

int dpni_set_qos_table(struct dpni *dpni, const struct dpni_qos_tbl_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	struct extract_data ext_data = { { 0 } };
	uint64_t ext_paddr = virt_to_phys(&ext_data);
	int err;
	
	/* prepare command */
	err = build_extract_cfg_extention(cfg->extract_cfg, &ext_data);
	if (err)
		return err;
	DPNI_CMD_SET_QOS_TABLE(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_QOS_TBL,
			  DPNI_CMDSZ_SET_QOS_TBL, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_delete_qos_table(struct dpni *dpni)
{
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_DELETE_QOS_TBL,
			  DPNI_CMDSZ_DELETE_QOS_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_add_qos_entry(struct dpni *dpni,
		       const struct dpni_key_cfg *cfg,
		       uint8_t tc_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	uint64_t key_paddr, mask_paddr;

	key_paddr = virt_to_phys(cfg->key);
	mask_paddr = virt_to_phys(cfg->mask);

	DPNI_CMD_ADD_QOS_ENTRY(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_ADD_QOS_ENT,
			  DPNI_CMDSZ_ADD_QOS_ENT, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_remove_qos_entry(struct dpni *dpni, const struct dpni_key_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	uint64_t key_paddr, mask_paddr;

	key_paddr = virt_to_phys(cfg->key);
	mask_paddr = virt_to_phys(cfg->mask);

	DPNI_CMD_REMOVE_QOS_ENTRY(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_REMOVE_QOS_ENT,
			  DPNI_CMDSZ_REMOVE_QOS_ENT, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_clear_qos_table(struct dpni *dpni)
{
	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_CLR_QOS_TBL,
			  DPNI_CMDSZ_CLR_QOS_TBL, CMDIF_PRI_LOW, NULL);
}

int dpni_set_fs_table(struct dpni *dpni,
		      uint8_t tc_id,
		      const struct dpni_fs_tbl_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPNI_CMD_SET_FS_TABLE(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_FS_TBL,
			  DPNI_CMDSZ_SET_FS_TBL, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_delete_fs_table(struct dpni *dpni, uint8_t tc_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_DELETE_FS_TABLE(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_DELETE_FS_TBL,
			  DPNI_CMDSZ_DELETE_FS_TBL, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_add_fs_entry(struct dpni *dpni,
		      uint8_t tc_id,
		      const struct dpni_key_cfg *cfg,
		      uint16_t flow_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	uint64_t key_paddr, mask_paddr;

	key_paddr = virt_to_phys(cfg->key);
	mask_paddr = virt_to_phys(cfg->mask);

	DPNI_CMD_ADD_FS_ENTRY(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_ADD_FS_ENT,
			  DPNI_CMDSZ_ADD_FS_ENT, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_remove_fs_entry(struct dpni *dpni,
			 uint8_t tc_id,
			 const struct dpni_key_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	uint64_t key_paddr, mask_paddr;

	key_paddr = virt_to_phys(cfg->key);
	mask_paddr = virt_to_phys(cfg->mask);

	DPNI_CMD_REMOVE_FS_ENTRY(CMD_PREP);

	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_REMOVE_FS_ENT,
			  DPNI_CMDSZ_REMOVE_FS_ENT, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_clear_fs_table(struct dpni *dpni, uint8_t tc_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_CLEAR_FS_TABLE(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_CLR_FS_TBL,
			  DPNI_CMDSZ_CLR_FS_TBL, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_irq(struct dpni *dpni,
		 uint8_t irq_index,
		 int *type,
		 uint64_t *irq_paddr,
		 uint32_t *irq_val,
		 int *irq_virt_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPNI_CMD_GET_IRQ(CMD_PREP);
	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_IRQ,
			 DPNI_CMDSZ_GET_IRQ, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpni_get_irq_enable(struct dpni *dpni,
			uint8_t irq_index,
			uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPNI_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_IRQ_ENABLE,
			 DPNI_CMDSZ_GET_IRQ_ENABLE, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dpni_set_irq_enable(struct dpni *dpni,
			uint8_t irq_index,
			uint8_t enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_SET_IRQ_ENABLE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_IRQ_ENABLE,
			  DPNI_CMDSZ_SET_IRQ_ENABLE, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_irq_mask(struct dpni *dpni, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPNI_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_IRQ_MASK,
			 DPNI_CMDSZ_GET_IRQ_MASK, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpni_set_irq_mask(struct dpni *dpni, uint8_t irq_index, uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_IRQ_MASK,
			  DPNI_CMDSZ_SET_IRQ_MASK, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_get_irq_status(struct dpni *dpni, uint8_t irq_index, uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPNI_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_GET_IRQ_STATUS,
			 DPNI_CMDSZ_GET_IRQ_STATUS, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPNI_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpni_clear_irq_status(struct dpni *dpni, uint8_t irq_index, uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPNI_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_CLEAR_IRQ_STATUS,
			  DPNI_CMDSZ_CLEAR_IRQ_STATUS, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpni_set_vlan_filters(struct dpni *dpni, int en)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	DPNI_CMD_SET_VLAN_FILTERS(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_VLAN_FILTERS,
			  DPNI_CMDSZ_SET_VLAN_FILTERS, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

#if 0
int dpni_set_vlan_insertion(struct dpni *dpni, int enable)
{
	struct mc_cmd_data cmd_data = { {0} };
	DPNI_CMD_SET_VLAN_INSERTION(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_VLAN_INSERTION,
		DPNI_CMDSZ_SET_VLAN_INSERTION, CMDIF_PRI_LOW,
		(uint8_t *)&cmd_data);
}

int dpni_set_vlan_removal(struct dpni *dpni, int enable)
{
	struct mc_cmd_data cmd_data = { {0} };
	DPNI_CMD_SET_VLAN_REMOVAL(CMD_PREP);
	return dplib_send(&(dpni->regs), (uint16_t)dpni->auth, DPNI_CMDID_SET_VLAN_REMOVAL,
		DPNI_CMDSZ_SET_VLAN_REMOVAL, CMDIF_PRI_LOW,
		(uint8_t *)&cmd_data);
}
#endif
