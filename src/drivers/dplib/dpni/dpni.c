
#include "common/types.h"
#include "common/fsl_string.h"
#include "common/fsl_cmdif.h"
#include "common/fsl_malloc.h"
#include "common/gen.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_dpni_cmd.h"
#include "common/fsl_stdio.h"
#include "dpni.h"


struct dpni *dpni_open(void *regs, int id)
{   
    struct dpni *dpni = (struct dpni *)fsl_os_malloc(sizeof(struct dpni));
    
    dpni->dev = cmdif_open(regs, FSL_OS_MOD_DPNI, (uint16_t) id);  		
    if (dpni->dev != NULL)
    	return dpni;
    return NULL;
}

int dpni_close(struct dpni *dpni)
{
	struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
	return cmdif_close(dev);
}

int dpni_defconfig(struct dpni_cfg *cfg)
{
	memset(cfg, 0, sizeof(struct dpni_cfg));
	return 0;
}

int dpni_init(struct dpni *dpni,
              const struct dpni_cfg *cfg,
              const struct dpni_init_params *params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
    /* write command body */
    prepare_init_cmd(desc, cfg, params);

    /* send command to mc*/
	return cmdif_send(dev, 
					  DPNI_CMD_INIT, 
					  DPNI_CMD_INIT_S, 
					  CMDIF_PRI_LOW, 
					  desc);
}

int dpni_free(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_FREE, 
    				  DPNI_CMD_FREE_S,
    				  CMDIF_PRI_LOW, 
    				  NULL);
}

int dpni_attach(struct dpni *dpni, const struct dpni_attach_params *params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
    /* write command body */
    prepare_attach_cmd(desc, params);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_ATTACH, 
    				  DPNI_CMD_ATTACH_S, 
    				  CMDIF_PRI_LOW, 
    				  desc);
}


int dpni_detach(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_DETACH,
    				  DPNI_CMD_DETACH_S,
    				  CMDIF_PRI_LOW, 
    				  NULL);
}


int dpni_set_tx_tc(struct dpni *dpni, const struct dpni_tx_tc_cfg *tc_cfg)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc =  (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_tx_tc_cmd(desc, tc_cfg);

    /* send command to mc*/
    return cmdif_send(dev, 
                      DPNI_CMD_SET_TX_TC, 
                      DPNI_CMD_SET_TX_TC_S, 
                      CMDIF_PRI_LOW,
                      desc);
}

int dpni_set_rx_tc(struct dpni *dpni, const struct dpni_rx_tc_cfg *tc_cfg)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_rx_tc_cmd(desc, tc_cfg);

    /* send command to mc*/
    return cmdif_send(dev, 
                      DPNI_CMD_SET_RX_TC, 
                      DPNI_CMD_SET_RX_TC_S,
                      CMDIF_PRI_LOW,
                      desc);    
}

int dpni_set_tx_queue_cfg(struct dpni *dpni,
                          uint8_t tcid,
                          const struct dpni_tx_queue_cfg *params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_tx_q_cfg_cmd(desc, tcid, params);

    /* send command to mc*/
    return cmdif_send(dev, 
                      DPNI_CMD_SET_TX_Q_CFG, 
                      DPNI_CMD_SET_TX_Q_CFG_S, 
                      CMDIF_PRI_LOW, 
                      desc);
}

int dpni_set_rx_flow_io(struct dpni			*dpni,
                        uint8_t				tcid,
                        uint8_t				flowid,
                        const struct dpni_rx_flow_io	*params)
{
	    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
	    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)
							    cmdif_get_desc(dev);

	    /* write command body */
	    prepare_rx_flow_io_cmd(desc, tcid, flowid, params);

	    /* send command to mc*/
	    return cmdif_send(dev, 
	                      DPNI_CMD_SET_RX_FLOW_IO, 
	                      DPNI_CMD_SET_RX_FLOW_IO_S,
	                      CMDIF_PRI_LOW, 
	                      desc);	
}

int dpni_set_rx_flow_cfg(struct dpni *dpni,
                          uint8_t tcid,
                          uint8_t flowid,
                          const struct dpni_rx_flow_cfg *params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_rx_flow_cfg_cmd(desc, tcid, flowid, params);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_SET_RX_FLOW_CFG, 
    				  DPNI_CMD_SET_RX_FLOW_CFG_S,
    				  CMDIF_PRI_LOW, 
    				  desc);
}
#if 0
int dpni_get_cfg(struct dpni *dpni, struct dpni_int_cfg *cfg)
{
	/* TODO - review */
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc; 
    int err;
    /* send command to mc*/
    err =  cmdif_send(dev, 
    				  DPNI_CMD_GET_CFG, 
    				  DPNI_CMD_GET_CFG_S,
    				  CMDIF_PRI_LOW, 
    				  NULL);
    if (!err)
    {
    	desc= (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
    	recieve_get_cfg_cmd(desc, cfg);	  
    }
    return err;
}
#endif
int dpni_get_attributes(struct dpni *dpni, 
			struct dpni_attributes *attributes)
{
	/* TODO - review */
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc;
    int err;
    /* send command to mc*/
    err = cmdif_send(dev, 
    				 DPNI_CMD_GET_ATTR, 
    				 DPNI_CMD_GET_ATTR_S,
    				 CMDIF_PRI_LOW, 
    				 NULL);
    if (!err)
    {
    	desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
    	recieve_get_attr_cmd(desc, attributes);     /* Read command body */
    }
    return err;
}

int dpni_get_qdid(struct dpni *dpni, uint16_t *qdid)
{
	/* TODO - review */
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc;
    int err;
    /* send command to mc*/
    err = cmdif_send(dev, 
    				DPNI_CMD_GET_QDID, 
    				DPNI_CMD_GET_QDID_S,
    				CMDIF_PRI_LOW, 
    				NULL);
    if (!err)
    {
    	desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
    	recieve_get_qdid_cmd(desc, qdid);     /* Read command body */
    }
    return err;
}

int dpni_enable(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_ENABLE, 
    				  DPNI_CMD_ENABLE_S, 
    				  CMDIF_PRI_LOW, 
    				  NULL);
}

int dpni_disable(struct dpni *dpni)
{
    struct cmdif_dev *dev =  (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev,
    				  DPNI_CMD_DISABLE, 
    				  DPNI_CMD_DISABLE_S,
    				  CMDIF_PRI_LOW,
    				  NULL);	
}

int dpni_get_tx_data_offset(struct dpni *dpni, uint16_t *data_offset)
{
	/* TODO - review */
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc;
    int err;
    /* send command to mc*/
    err = cmdif_send(dev, 
    				 DPNI_CMD_GET_TX_DATA_OFFSET, 
    				 DPNI_CMD_GET_TX_DATA_OFF_S,
    				 CMDIF_PRI_LOW, 
    				 NULL);
    if (!err)
    {
    	desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
    	recieve_get_tx_data_offset_cmd(desc, data_offset);     /* Read command body */
    }
    return err;
}

int dpni_get_stats(struct dpni *dpni, struct dpni_stats *stats)
{
	/* TODO - fill stats descriptor */
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc;
    int err;
    /* send command to mc*/
    err = cmdif_send(dev, 
    				  DPNI_CMD_GET_STATS, 
    				  DPNI_CMD_GET_STATS_S,
    				  CMDIF_PRI_LOW, 
    				  NULL);
    if (!err)
    {
    	desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
        recieve_get_stats_cmd(desc, stats);
    }
    return err;
}

int dpni_reset_stats(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_RST_STATS, 
    				  DPNI_CMD_RST_STATS_S,
    				  CMDIF_PRI_LOW, 
    				  NULL);	
}

int dpni_get_link_state(struct dpni *dpni)
{
	/* TODO - how to get the state? */
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_GET_LNK_STATE, 
    				  DPNI_CMD_GET_LNK_STATE_S, 
    				  CMDIF_PRI_LOW,
    				  NULL);
}

int dpni_set_mfl(struct dpni *dpni, uint16_t mfl)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_set_mtu_cmd(desc, mfl);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_SET_MFL, 
    				  DPNI_CMD_SET_MFL_S,
    				  CMDIF_PRI_LOW, 
    				  desc);
}

int dpni_get_mfl(struct dpni *dpni, uint16_t *mfl)
{
	/* TODO - review */
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc;
    int err;
    /* send command to mc*/
    err = cmdif_send(dev, 
    				 DPNI_CMD_GET_MFL, 
    				 DPNI_CMD_GET_MFL_S,
    				 CMDIF_PRI_LOW, 
    				 NULL);
    if (!err)
    {
    	desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);
    	recieve_get_mfl_cmd(desc, mfl);     /* Read command body */
    }
    return err;
}

int dpni_set_mtu(struct dpni *dpni, uint16_t mtu)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_set_mtu_cmd(desc, mtu);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_SET_MTU, 
    				  DPNI_CMD_SET_MTU_S,
    				  CMDIF_PRI_LOW, 
    				  desc);
}

int dpni_enable_multicast_promisc(struct dpni *dpni, int en)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_mcast_promisc_cmd(desc, en);

    /* send command to mc*/
    return cmdif_send(dev,
    				  DPNI_CMD_MCAST_PROMISC, 
    				  DPNI_CMD_MCAST_PROMISC_S,
    				  CMDIF_PRI_LOW, 
    				  desc);
}

int dpni_modify_primary_mac_addr(struct dpni *dpni, uint8_t addr[])
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_mod_prim_mac_cmd(desc, addr);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_MODIFY_PRIM_MAC, 
    				  DPNI_CMD_MODIFY_PRIM_MAC_S, 
    				  CMDIF_PRI_LOW,
    				  desc);	
}

int dpni_add_mac_addr(struct dpni *dpni, const uint8_t addr[]) 
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc =  (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_add_mac_cmd(desc, addr);

    /* send command to mc*/
    return cmdif_send(dev,
    				  DPNI_CMD_ADD_MAC_ADDR, 
    				  DPNI_CMD_ADD_MAC_ADDR_S, 
    				  CMDIF_PRI_LOW, 
    				  desc);	
}

int dpni_remove_mac_addr(struct dpni *dpni, const uint8_t addr[]) 
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_remove_mac_cmd(desc, addr);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_REMOVE_MAC_ADDR, 
    				  DPNI_CMD_REMOVE_MAC_ADDR_S, 
    				  CMDIF_PRI_LOW, 
    				  desc);
}

int dpni_clear_mac_table(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_CLR_MAC_TBL, 
    				  DPNI_CMD_CLR_MAC_TBL_S,
    				  CMDIF_PRI_LOW, NULL);
}

int dpni_add_vlan_id(struct dpni *dpni, uint16_t vid)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_add_vlan_id_cmd(desc, vid);

    /* send command to mc*/
    return cmdif_send(dev,
    				  DPNI_CMD_ADD_VLAN_ID, 
    				  DPNI_CMD_ADD_VLAN_ID_S,
    				  CMDIF_PRI_LOW, 
    				  desc);	
}

int dpni_remove_vlan_id(struct dpni *dpni, uint16_t vid)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_remove_vlan_id_cmd(desc, vid);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_REMOVE_VLAN_ID, 
    				  DPNI_CMD_REMOVE_VLAN_ID_S,
    				  CMDIF_PRI_LOW,
    				  desc);	
}

int dpni_clear_vlan_table(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_CLR_VLAN_TBL, 
    				  DPNI_CMD_CLR_VLAN_TBL_S,
    				  CMDIF_PRI_LOW, 
    				  NULL);
}

int dpni_set_qos_table(struct dpni *dpni,
                       const struct dpni_qos_tbl_params *params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_set_qos_tbl_cmd(desc, params);

    /* send command to mc*/
    return cmdif_send(dev,
    				  DPNI_CMD_SET_QOS_TBL, 
    				  DPNI_CMD_SET_QOS_TBL_S,
    				  CMDIF_PRI_LOW, 
    				  desc);	
}

int dpni_delete_qos_table(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_DELETE_QOS_TBL, 
    				  DPNI_CMD_DELETE_QOS_TBL_S, 
    				  CMDIF_PRI_LOW, 
    				  NULL);	
}

int dpni_add_qos_entry(struct dpni *dpni,
                       const struct dpni_key_params *key_params, 
                       uint8_t tcid)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_add_qos_entry_cmd(desc, key_params, tcid);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_ADD_QOS_ENT, 
    				  DPNI_CMD_ADD_QOS_ENT_S,
    				  CMDIF_PRI_LOW,
    				  desc);
}

int dpni_remove_qos_entry(struct dpni *dpni, 
                          const struct dpni_key_params *key_params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_remove_qos_entry_cmd(desc, key_params);

    /* send command to mc*/
    return cmdif_send(dev, 
                      DPNI_CMD_REMOVE_QOS_ENT, 
                      DPNI_CMD_REMOVE_QOS_ENT_S, 
                      CMDIF_PRI_LOW, 
                      desc);
}

int dpni_clear_qos_table(struct dpni *dpni)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;


    /* send command to mc*/
    return cmdif_send(dev, 
                      DPNI_CMD_CLR_QOS_TBL, 
                      DPNI_CMD_CLR_QOS_TBL_S,
                      CMDIF_PRI_LOW,
                      NULL);
}

int dpni_set_dist(struct dpni *dpni, 
                  const struct dpni_dist_params dist[])
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_set_dist_cmd(desc, dist);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_SET_DIST, 
    				  DPNI_CMD_SET_DIST_S, 
    				  CMDIF_PRI_LOW, 
    				  desc);
}

int dpni_set_fs_table(struct dpni *dpni, 
                      uint8_t 				tcid,
                      const struct dpni_fs_tbl_params *params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_set_fs_tbl_cmd(desc, tcid, params);

    /* send command to mc*/
    return cmdif_send(dev, 
                      DPNI_CMD_SET_FS_TBL, 
                      DPNI_CMD_SET_FS_TBL_S,
                      CMDIF_PRI_LOW, 
                      desc);
}

int dpni_delete_fs_table(struct dpni *dpni, uint8_t tcid)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_delete_fs_tbl_cmd(desc, tcid);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_DELETE_FS_TBL, 
    				  DPNI_CMD_DELETE_FS_TBL_S,
    				  CMDIF_PRI_LOW,
    				  desc);
}

int dpni_add_fs_entry(struct dpni *dpni, 
                      uint8_t tcid,
                      const struct dpni_key_params *key_params, 
                      uint16_t flowid)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_add_fs_entry_cmd(desc, tcid, key_params, flowid);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_ADD_FS_ENT, 
    				  DPNI_CMD_ADD_FS_ENT_S,
    				  CMDIF_PRI_LOW,
    				  desc);
}

int dpni_remove_fs_entry(struct dpni *dpni, 
			uint8_t tcid, 
			const struct dpni_key_params *key_params)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_remove_fs_entry_cmd(desc, tcid, key_params);

    /* send command to mc*/
    return cmdif_send(dev,
    				  DPNI_CMD_REMOVE_FS_ENT, 
    				  DPNI_CMD_REMOVE_FS_ENT_S,
    				  CMDIF_PRI_LOW,
    				  desc);
}

int dpni_clear_fs_table(struct dpni *dpni, uint8_t tcid)
{
    struct cmdif_dev *dev = (struct cmdif_dev *)dpni->dev;
    struct cmdif_cmd_desc *desc = (struct cmdif_cmd_desc *)cmdif_get_desc(dev);

    /* write command body */
    prepare_clear_fs_tbl_cmd(desc, tcid);

    /* send command to mc*/
    return cmdif_send(dev, 
    				  DPNI_CMD_CLR_FS_TBL, 
    				  DPNI_CMD_CLR_FS_TBL_S, 
    				  CMDIF_PRI_LOW, 
    				  desc);
}

void prepare_init_cmd(struct cmdif_cmd_desc	*desc,
                      const struct dpni_cfg *cfg,
                      const struct dpni_init_params *params)
{
	int i;
	uint64_t cmd_param = 0;
	/* build param 1*/
	u64_write_field(cmd_param, 
					DPNI_INIT_MAX_TCS_O, 
					DPNI_INIT_MAX_TCS_S, 	
                    cfg->max_tcs);
	u64_write_field(cmd_param, 
					DPNI_INIT_TYPE_O,
					DPNI_INIT_TYPE_S, 	
                    params->type);
	u64_write_field(cmd_param, 
					DPNI_INIT_DIST_KEY_VAR_O,
					DPNI_INIT_DIST_KEY_VAR_S, 	
                    cfg->dist_key_variety);
	for ( i = 0 ; i < NET_HDR_FLD_ETH_ADDR_SIZE ; i++ )
		    u64_write_field(cmd_param, 
		    		DPNI_INIT_MAC_ADDR_O + i * DPNI_INIT_MAC_ADDR_S, 
		    		DPNI_INIT_MAC_ADDR_S, 
		    		params->mac_addr[i]);

fsl_os_print("[%d] 0x%016x\n",__LINE__, cmd_param);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
	/* build param 2*/
    cmd_param = 0;
    u64_write_field(cmd_param, 
    				DPNI_INIT_OPTIONS_O, 
    				DPNI_INIT_OPTIONS_S, 	
                    cfg->options);
    GPP_CMD_WRITE_PARAM(desc, 2, cmd_param);
	/* build param 3*/
    cmd_param = 0;
    for ( i = 0 ; i < DPNI_MAX_NUM_OF_TC ; i ++ )
    {
	    u64_write_field(cmd_param, 
	    		DPNI_INIT_MAX_DIST_O + i * DPNI_INIT_MAX_DIST_S,
	    		DPNI_INIT_MAX_DIST_S,
	    		cfg->max_dist_per_tc[i]);
    }
    GPP_CMD_WRITE_PARAM(desc, 3, cmd_param);
	/* build param 4*/
    cmd_param = 0;
    u64_write_field(cmd_param, 
    				DPNI_INIT_MAX_UNICST_FILTR_O, 
    				DPNI_INIT_MAX_UNICST_FILTR_S, 	
                    cfg->max_unicast_filters);
    u64_write_field(cmd_param, 
    				DPNI_INIT_MAX_MLTICST_FILTR_O, 
    				DPNI_INIT_MAX_MLTICST_FILTR_S, 	
                    cfg->max_multicast_filters);
    u64_write_field(cmd_param, 
    				DPNI_INIT_MAX_VLAN_FILTR_O, 
    				DPNI_INIT_MAX_VLAN_FILTR_S, 	
                    cfg->max_vlan_filters);
    u64_write_field(cmd_param, 
    				DPNI_INIT_MAX_QOS_ENTR_O, 
    				DPNI_INIT_MAX_QOS_ENTR_S, 	
                    cfg->max_qos_entries);
    u64_write_field(cmd_param, 
    				DPNI_INIT_MAX_QOS_KEY_SIZE_O, 
    				DPNI_INIT_MAX_QOS_KEY_SIZE_S, 	
                    cfg->max_qos_key_size);
    u64_write_field(cmd_param, 
    				DPNI_INIT_MAX_DIST_KEY_SIZE_O, 
    				DPNI_INIT_MAX_DIST_KEY_SIZE_S, 	
                    cfg->max_dist_key_size);
    GPP_CMD_WRITE_PARAM(desc, 4, cmd_param);
}

void prepare_attach_cmd(struct cmdif_cmd_desc *desc,
                        const struct dpni_attach_params *params)
{
	/* TODO - review - union */
    uint64_t cmd_param = 0;
    /* build param 1*/
    u64_write_field(cmd_param, 
    				DPNI_ATTACH_DPIO_ID_O,
    				DPNI_ATTACH_DPIO_ID_S, 
                    (uint16_t)params->dpio_id);
    u64_write_field(cmd_param,
    				DPNI_ATTACH_DPSP_ID_O, 
    				DPNI_ATTACH_DPSP_ID_S, 
    				(uint16_t)params->dpsp_id);
#ifndef AIOP
    u64_write_field(cmd_param, 
    				DPNI_ATTACH_STASH_EN_O, 
    				DPNI_ATTACH_STASH_EN_S,
    				params->flc->stash_en);
    u64_write_field(cmd_param, 
    				DPNI_ATTACH_DAN_EN_O, 
    				DPNI_ATTACH_DAN_EN_S, 
     				params->dan_en);
    if (params->flc->stash_en)
    {
    	u64_write_field(cmd_param, 
    					DPNI_ATTACH_FRAME_ANNO_O, 
    					DPNI_ATTACH_FRAME_ANNO_S, 
    					params->flc->u.stash.frame_annotation_size);
    	u64_write_field(cmd_param, 
    					DPNI_ATTACH_FRAME_DATA_O, 
    					DPNI_ATTACH_FRAME_DATA_S, 
    					params->flc->u.stash.frame_data_size);
    	u64_write_field(cmd_param,
    					DPNI_ATTACH_FLOW_CTX_S_O, 
    					DPNI_ATTACH_FLOW_CTX_S_S,
    					params->flc->u.stash.flow_context_size);
    }
#endif /* AIOP */
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
	/* build param 2*/
    cmd_param = 0;
    u64_write_field(cmd_param, 
    				DPNI_ATTACH_RX_USR_CTX_O,
    				DPNI_ATTACH_RX_USR_CTX_S,
    				params->rx_user_ctx);
    GPP_CMD_WRITE_PARAM(desc, 2, cmd_param);
	/* build param 3*/
    cmd_param = 0;
    u64_write_field(cmd_param,
    				DPNI_ATTACH_RX_ERR_USR_CTX_O, 
    				DPNI_ATTACH_RX_ERR_USR_CTX_S, 
    				params->rx_err_user_ctx);
    GPP_CMD_WRITE_PARAM(desc, 3, cmd_param);
	/* build param 4*/
    cmd_param = 0;
    u64_write_field(cmd_param,
    				DPNI_ATTACH_TX_USR_CTX_O, 
    				DPNI_ATTACH_TX_USR_CTX_S,
    				params->tx_err_user_ctx);
    GPP_CMD_WRITE_PARAM(desc, 4, cmd_param);
	/* build param 5*/
    cmd_param = 0;
    u64_write_field(cmd_param, 
    				DPNI_ATTACH_TX_CONF_USR_CTX_O, 
    				DPNI_ATTACH_TX_CONF_USR_CTX_O,
    				params->tx_conf_user_ctx);
    GPP_CMD_WRITE_PARAM(desc, 5, cmd_param);
	/* build param 6*/
    cmd_param = 0;
#ifndef AIOP
    if (params->flc->stash_en)
    {
        u64_write_field(cmd_param, 
        				DPNI_ATTACH_FLOW_CTX_ADDR_O,
        				DPNI_ATTACH_FLOW_CTX_ADDR_S,
        				params->flc->u.stash.flow_context_addr);
    }
    else
    {
    	u64_write_field(cmd_param, 
    					DPNI_ATTACH_ODP_O,
    					DPNI_ATTACH_ODP_S, 
       					params->flc->u.odp);
    }
#endif /* AIOP */
    GPP_CMD_WRITE_PARAM(desc, 6, cmd_param);
}

void prepare_tx_tc_cmd(struct cmdif_cmd_desc *desc, 
                       const struct dpni_tx_tc_cfg *tc_cfg)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    /*TODO */

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_rx_tc_cmd(struct cmdif_cmd_desc *desc, 
                       const struct dpni_rx_tc_cfg *tc_cfg)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, 
    				DPNI_SET_RX_TC_DIST_O,
    				DPNI_SET_RX_TC_DIST_S, 
                    tc_cfg->dist_size);

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_tx_q_cfg_cmd(struct cmdif_cmd_desc *desc,
                          uint8_t tcid,
                          const struct dpni_tx_queue_cfg *params)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, 
    				DPNI_SET_TX_Q_CFG_DPIO_O, 
    				DPNI_SET_TX_Q_CFG_DPIO_S,
    				(uint16_t)params->tx_conf_dpio_id);
    u64_write_field(cmd_param,
    				DPNI_SET_TX_Q_CFG_TCID_O,
    				DPNI_SET_TX_Q_CFG_TCID_S, 
    				tcid);
    u64_write_field(cmd_param,
    				DPNI_SET_TX_Q_CFG_TX_CONF_O,
    				DPNI_SET_TX_Q_CFG_TX_CONF_S,
    				params->tx_conf);
    u64_write_field(cmd_param, 
    				DPNI_SET_TX_Q_CFG_CKSUM_GEN_O, 
    				DPNI_SET_TX_Q_CFG_CKSUM_GEN_S, 
    				params->chksum_gen);
    u64_write_field(cmd_param,
    				DPNI_SET_TX_Q_CFG_DEPTH_O, 
    				DPNI_SET_TX_Q_CFG_DEPTH_S, 
    				params->depth_limit);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
    /* build param 2 */
    cmd_param = 0;
    u64_write_field(cmd_param, 
    				DPNI_SET_TX_Q_CFG_USR_CTX_O,
    				DPNI_SET_TX_Q_CFG_USR_CTX_S, 
    				params->user_ctx);
    GPP_CMD_WRITE_PARAM(desc, 2, cmd_param);
    /* build param 3 */
    cmd_param = 0;
    u64_write_field(cmd_param,
    				DPNI_SET_TX_Q_CFG_MOD_OPT_O,
    				DPNI_SET_TX_Q_CFG_MOD_OPT_S,
    				params->options);
    GPP_CMD_WRITE_PARAM(desc, 3, cmd_param);  
}

void prepare_rx_flow_io_cmd(struct cmdif_cmd_desc *desc, 
                          uint8_t tcid,
                          uint8_t flowid,
                          const struct dpni_rx_flow_io *params)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    u64_write_field(cmd_param, 
                    DPNI_SET_RX_FLOW_IO_DPIO_O,
                    DPNI_SET_RX_FLOW_IO_DPIO_S,
                    params->dpio_id);
    u64_write_field(cmd_param,
                    DPNI_SET_RX_FLOW_IO_FLOWID_O,
                    DPNI_SET_RX_FLOW_IO_FLOWID_S, 
                    flowid);
    u64_write_field(cmd_param, 
                    DPNI_SET_RX_FLOW_IO_TCID_O, 
                    DPNI_SET_RX_FLOW_IO_TCID_S,
                    tcid);
    u64_write_field(cmd_param, 
                    DPNI_SET_RX_FLOW_IO_OPTIONS_O, 
                    DPNI_SET_RX_FLOW_IO_OPTIONS_S, 
                    params->options);
}

void prepare_rx_flow_cfg_cmd(struct cmdif_cmd_desc *desc, 
                          uint8_t tcid,
                          uint8_t flowid,
                          const struct dpni_rx_flow_cfg *params)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, 
                    DPNI_SET_RX_FLOW_CFG_DAN_EN_O,
                    DPNI_SET_RX_FLOW_CFG_DAN_EN_S,
                    params->dan_en);
    u64_write_field(cmd_param,
                    DPNI_SET_RX_FLOW_CFG_FLOWID_O,
                    DPNI_SET_RX_FLOW_CFG_FLOWID_S, 
                    flowid);
    u64_write_field(cmd_param, 
                    DPNI_SET_RX_FLOW_CFG_TCID_O, 
                    DPNI_SET_RX_FLOW_CFG_TCID_S,
                    tcid);
    u64_write_field(cmd_param, 
                    DPNI_SET_RX_FLOW_CFG_OPTIONS_O, 
                    DPNI_SET_RX_FLOW_CFG_OPTIONS_S, 
                    params->options);
#ifndef AIOP
    u64_write_field(cmd_param,
    				DPNI_SET_RX_Q_CTX_STASH_EN_O, 
    				DPNI_SET_RX_Q_CTX_STASH_EN_S,
    				params->flc->stash_en);
    if (params->flc->stash_en) /*TODO - review */
    {
    	u64_write_field(cmd_param, 
    					DPNI_SET_RX_Q_CTX_FRM_ANNO_O, 
    					DPNI_SET_RX_Q_CTX_FRM_ANNO_S,
    					params->flc->u.stash.frame_annotation_size);
    	u64_write_field(cmd_param,
    					DPNI_SET_RX_Q_CTX_FRM_DATA_O,
    					DPNI_SET_RX_Q_CTX_FRM_DATA_S,
    					params->flc->u.stash.frame_data_size);
    	u64_write_field(cmd_param, 
    					DPNI_SET_RX_Q_CTX_FLOW_CTX_S_O, 
    					DPNI_SET_RX_Q_CTX_FLOW_CTX_S_S,
    					params->flc->u.stash.flow_context_size);
    }
#endif /* AIOP */
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
    
    /* build param 2 */
    cmd_param = 0;
    u64_write_field(cmd_param, 
                    DPNI_SET_RX_FLOW_CFG_USR_CTX_O, 
                    DPNI_SET_RX_FLOW_CFG_USR_CTX_S, 
                    params->user_ctx);
    GPP_CMD_WRITE_PARAM(desc, 2, cmd_param);
}

#if 0
void recieve_get_cfg_cmd(struct cmdif_cmd_desc *desc, 
						 struct dpni_int_cfg *cfg)
{
    uint64_t cmd_param = GPP_CMD_READ_PARAM(desc, 1);
    
    /* read param 1*/
    cfg->tx_qdid = (uint16_t)u64_read_field(cmd_param, 
    										DPNI_GET_CFG_TX_QDID_O,
    										DPNI_GET_CFG_TX_QDID_S);
    cfg->tx_data_offset = (uint16_t)u64_read_field(cmd_param, 
    											   DPNI_GET_CFG_TX_DATA_O, 
    											   DPNI_GET_CFG_TX_DATA_S);    
}
#endif /* 0 */

void recieve_get_attr_cmd(struct cmdif_cmd_desc *desc, 
                          struct dpni_attributes *attr)
{
	int i;
    uint64_t cmd_param = GPP_CMD_READ_PARAM(desc, 1);
    
    /* read param 1*/
    attr->max_tcs = (uint8_t)u64_read_field(cmd_param,
    					DPNI_GET_ATTR_MAX_TCS_O,
    					DPNI_GET_ATTR_MAX_TCS_S);
    /* read param 2 */
	cmd_param = GPP_CMD_READ_PARAM(desc, 2);
    attr->options = (uint64_t)u64_read_field(cmd_param, 
    					 DPNI_GET_ATTR_OPTIONS_O, 
    					 DPNI_GET_ATTR_OPTIONS_S);
    /* read param 3 */
	cmd_param = GPP_CMD_READ_PARAM(desc, 3);
    for ( i = 0 ; i < DPNI_MAX_NUM_OF_TC ; i ++ )
    {
    	attr->max_dist_per_tc[i] = (uint8_t)u64_read_field(cmd_param, 
    			DPNI_GET_ATTR_MAX_DIST_O + i * DPNI_GET_ATTR_MAX_DIST_S, 
    			DPNI_GET_ATTR_MAX_DIST_S);    
    }
}

void recieve_get_qdid_cmd(struct cmdif_cmd_desc *desc, 
                          uint16_t		*qdid)
{
    uint64_t cmd_param = GPP_CMD_READ_PARAM(desc, 1);
    
    /* read param 1*/
    *qdid = (uint16_t)u64_read_field(cmd_param,
    				 DPNI_GET_QDID_O,
    				 DPNI_GET_QDID_S);
}

void recieve_get_tx_data_offset_cmd(struct cmdif_cmd_desc *desc, 
                                    uint16_t		  *offset)
{
    uint64_t cmd_param = GPP_CMD_READ_PARAM(desc, 1);
    
    /* read param 1*/
    *offset = (uint16_t)u64_read_field(cmd_param,
    					DPNI_GET_TX_DATA_OFF_O,
    					DPNI_GET_TX_DATA_OFF_S);
}

void recieve_get_mfl_cmd(struct cmdif_cmd_desc *desc, 
			uint16_t		  *mfl)
{
    uint64_t cmd_param = GPP_CMD_READ_PARAM(desc, 1);
    
    /* read param 1*/
    *mfl = (uint16_t)u64_read_field(cmd_param,
    				DPNI_GET_MFL_O,
    				DPNI_GET_MFL_S);
}

void prepare_set_mfl_cmd(struct cmdif_cmd_desc *desc, uint16_t mfl)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, DPNI_SET_MFL_MFL_O, DPNI_SET_MFL_MFL_S, mfl);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_set_mtu_cmd(struct cmdif_cmd_desc *desc, uint16_t mtu)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, DPNI_SET_MTU_MTU_O, DPNI_SET_MTU_MTU_S, mtu);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_mcast_promisc_cmd(struct cmdif_cmd_desc *desc, int en)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    u64_write_field(cmd_param, DPNI_EN_MCAST_EN_O, DPNI_EN_MCAST_EN_S, en);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_mod_prim_mac_cmd(struct cmdif_cmd_desc *desc, 
                              uint8_t addr[6])
{
	int i;
    uint64_t cmd_param = 0;

    /* build param 1*/
    for ( i = 0 ; i < NET_HDR_FLD_ETH_ADDR_SIZE ; i++ )
    {
	    u64_write_field(cmd_param,
	    	DPNI_MODIFY_PRIM_MAC_ADDR_O + i * DPNI_MODIFY_PRIM_MAC_ADDR_S, 
	    	DPNI_MODIFY_PRIM_MAC_ADDR_S, 
	    	addr[i]);
    }
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_add_mac_cmd(struct cmdif_cmd_desc *desc, const uint8_t addr[])
{
	int i;
    uint64_t cmd_param = 0;

    /* build param 1*/
    for ( i = 0 ; i < NET_HDR_FLD_ETH_ADDR_SIZE ; i++ )
    {
	    u64_write_field(cmd_param, 
	    		DPNI_ADD_MAC_ADDR_O + i * DPNI_ADD_MAC_ADDR_S,
	    		DPNI_ADD_MAC_ADDR_S,
	    		addr[i]);
    }

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_remove_mac_cmd(struct cmdif_cmd_desc *desc, const uint8_t addr[])
{
	int i;
    uint64_t cmd_param = 0;

    /* build param 1*/
    for ( i = 0 ; i < NET_HDR_FLD_ETH_ADDR_SIZE ; i++ )
    {
	    u64_write_field(cmd_param, 
	    		DPNI_REMOVE_MAC_ADDR_O + i * DPNI_REMOVE_MAC_ADDR_S,
	    		DPNI_REMOVE_MAC_ADDR_S, 
	    		addr[i]);
    }
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_add_vlan_id_cmd(struct cmdif_cmd_desc *desc, uint16_t vid)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, 
    			DPNI_ADD_VLAN_ID_VID_O,
    			DPNI_ADD_VLAN_ID_VID_S,
    			vid);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_remove_vlan_id_cmd(struct cmdif_cmd_desc *desc, uint16_t vid)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, 
    			DPNI_REMOVE_VLAN_ID_VID_O,
    			DPNI_REMOVE_VLAN_ID_VID_S,
    			vid);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_set_qos_tbl_cmd(struct cmdif_cmd_desc *desc, 
                             const struct dpni_qos_tbl_params *params)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    u64_write_field(cmd_param, 
    				DPNI_SET_QOS_TBL_DROP_FRAME_O,
    				DPNI_SET_QOS_TBL_DROP_FRAME_S,
    				params->drop_frame);
    u64_write_field(cmd_param, 
    				DPNI_SET_QOS_TBL_DEFAULT_TC_O,
    				DPNI_SET_QOS_TBL_DEFAULT_TC_S, 
    				params->default_tc);

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_add_qos_entry_cmd(struct cmdif_cmd_desc *desc, 
                               const struct dpni_key_params *params,
                               uint8_t tcid)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    /* TODO */
    UNUSED (params);
    UNUSED (tcid);

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_remove_qos_entry_cmd(struct cmdif_cmd_desc *desc, 
                                  const struct dpni_key_params *params)
{
    uint64_t cmd_param = 0;

    /* build param 1*/
    /* TODO */
    UNUSED (params);
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_set_dist_cmd(struct cmdif_cmd_desc *desc, 
                          const struct dpni_dist_params *dist)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    u64_write_field(cmd_param, 
    				DPNI_SET_DIST_DIST_FS_O, 
    				DPNI_SET_DIST_DIST_FS_S, 
    				dist->dist_fs);
    /* TODO */

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_set_fs_tbl_cmd(struct cmdif_cmd_desc 		*desc,
                            uint8_t 				tcid,
                            const struct dpni_fs_tbl_params 	*params)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    u64_write_field(cmd_param,
    				DPNI_SET_FS_TBL_DEFAULT_FLOWID_O, 
    				DPNI_SET_FS_TBL_DEFAULT_FLOWID_S, 
    				params->default_flowid);
    u64_write_field(cmd_param, 
    				DPNI_SET_FS_TBL_DROP_FRAME_O,
    				DPNI_SET_FS_TBL_DROP_FRAME_S,
    				params->drop_frame);
    u64_write_field(cmd_param,
    				DPNI_SET_FS_TBL_TCID_O,
    				DPNI_SET_FS_TBL_TCID_S, 
    				tcid);

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_delete_fs_tbl_cmd(struct cmdif_cmd_desc *desc, uint8_t tcid)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    u64_write_field(cmd_param, 
    				DPNI_DELETE_FS_TBL_TCID_O,
    				DPNI_DELETE_FS_TBL_TCID_S,
    				tcid);

    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_add_fs_entry_cmd(struct cmdif_cmd_desc *desc, 
                              uint8_t tcid,
                              const struct dpni_key_params *params,
                              uint16_t flowid)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    UNUSED (params);
    u64_write_field(cmd_param,
    				DPNI_ADD_FS_ENT_FLOWID_O, 
    				DPNI_ADD_FS_ENT_FLOWID_S,
    				flowid);
    u64_write_field(cmd_param,
    				DPNI_ADD_FS_ENT_TCID_O,
    				DPNI_ADD_FS_ENT_TCID_S, 
    				tcid);
    /* TODO */
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_remove_fs_entry_cmd(struct cmdif_cmd_desc *desc, 
				 uint8_t tcid, 
				 const struct dpni_key_params *params)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    UNUSED (params);
    u64_write_field(cmd_param, 
    				DPNI_REMOVE_FS_ENT_TCID_O, 
    				DPNI_REMOVE_FS_ENT_TCID_S, 
    				tcid);
    /* TODO */
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void prepare_clear_fs_tbl_cmd(struct cmdif_cmd_desc *desc, uint8_t tcid)
{
    uint64_t cmd_param = 0;
    /* build param 1*/
    u64_write_field(cmd_param, 
    				DPNI_CLR_FS_TBL_TCID_O, 
    				DPNI_CLR_FS_TBL_TCID_S, 
    				tcid);
    /* TODO */
    GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

void recieve_get_stats_cmd(struct cmdif_cmd_desc *desc,
                           const struct dpni_stats *stats)
{
    UNUSED (desc);
    UNUSED (stats);
    /* TODO */
}
