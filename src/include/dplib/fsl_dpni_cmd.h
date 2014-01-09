/**************************************************************************//*
 @File          fsl_dpni_cmd.h

 @Description   defines dpni portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPNI_CMD_H
#define _FSL_DPNI_CMD_H


/* cmd IDs */
#define DPNI_CMD_OPEN				0x801 
#define DPNI_CMD_CLOSE				0x121
#define DPNI_CMD_INIT				0x123
#define DPNI_CMD_FREE				0x124
#define DPNI_CMD_ATTACH				0x125
#define DPNI_CMD_DETACH				0x126
#define DPNI_CMD_ATTACH_LINK			0x127
#define DPNI_CMD_DETACH_LINK			0x128
#define DPNI_CMD_SET_TX_TC  			0x129
#define DPNI_CMD_SET_RX_TC  			0x12A
#define DPNI_CMD_SET_TX_Q_CFG			0x12B
#define DPNI_CMD_SET_RX_FLOW_CFG		0x12C
#define DPNI_CMD_SET_RX_FLOW_IO			0x14A
#define DPNI_CMD_GET_CFG			0x12D
#define DPNI_CMD_GET_ATTR			0x12E
#define DPNI_CMD_GET_QDID			0x12F
#define DPNI_CMD_ENABLE				0x130
#define DPNI_CMD_DISABLE			0x131
#define DPNI_CMD_GET_STATS			0x132
#define DPNI_CMD_RST_STATS			0x133
#define DPNI_CMD_GET_LNK_STATE			0x134
#define DPNI_CMD_SET_MFL			0x135
#define DPNI_CMD_SET_MTU			0x136
#define DPNI_CMD_MCAST_PROMISC			0x137
#define DPNI_CMD_SET_PRIM_MAC 		0x138
#define DPNI_CMD_ADD_MAC_ADDR			0x139
#define DPNI_CMD_REMOVE_MAC_ADDR		0x13A
#define DPNI_CMD_CLR_MAC_TBL			0x13B
#define DPNI_CMD_ADD_VLAN_ID			0x13C
#define DPNI_CMD_REMOVE_VLAN_ID			0x13D
#define DPNI_CMD_CLR_VLAN_TBL			0x13E
#define DPNI_CMD_SET_QOS_TBL			0x13F
#define DPNI_CMD_DELETE_QOS_TBL			0x140
#define DPNI_CMD_ADD_QOS_ENT			0x141
#define DPNI_CMD_REMOVE_QOS_ENT			0x142
#define DPNI_CMD_CLR_QOS_TBL			0x143
#define DPNI_CMD_SET_DIST			0x144
#define DPNI_CMD_SET_FS_TBL			0x145
#define DPNI_CMD_DELETE_FS_TBL			0x146
#define DPNI_CMD_ADD_FS_ENT			0x147
#define DPNI_CMD_REMOVE_FS_ENT			0x148
#define DPNI_CMD_CLR_FS_TBL			0x149
#define DPNI_CMD_GET_TX_DATA_OFFSET		0x150
#define DPNI_CMD_GET_PRIM_MAC 			0x151
#define DPNI_CMD_GET_MFL			0x152

/* cmd sizes */
#define DPNI_CMD_INIT_S				(8 * 3)
#define DPNI_CMD_FREE_S				0
#define DPNI_CMD_ATTACH_S			(8 * 6)
#define DPNI_CMD_DETACH_S			0
#define DPNI_CMD_ATTACH_LINK_S			8
#define DPNI_CMD_DETACH_LINK_S			0
#define DPNI_CMD_SET_TX_TC_S 			8
#define DPNI_CMD_SET_RX_TC_S 			8
#define DPNI_CMD_SET_TX_Q_CFG_S			(8 * 3)
#define DPNI_CMD_SET_RX_FLOW_CFG_S		(8 * 2)
#define DPNI_CMD_SET_RX_FLOW_IO_S		8
#define DPNI_CMD_GET_CFG_S			(8 * 2)
#define DPNI_CMD_GET_ATTR_S			(8 * 3)
#define DPNI_CMD_GET_QDID_S			8
#define DPNI_CMD_ENABLE_S			0
#define DPNI_CMD_DISABLE_S			0
#define DPNI_CMD_GET_TX_DATA_OFF_S		8
#define DPNI_CMD_GET_STATS_S			0
#define DPNI_CMD_RST_STATS_S			0
#define DPNI_CMD_GET_LNK_STATE_S		0
#define DPNI_CMD_SET_MFL_S			8
#define DPNI_CMD_SET_MTU_S			8
#define DPNI_CMD_MCAST_PROMISC_S		8
#define DPNI_CMD_SET_PRIM_MAC_S 		8
#define DPNI_CMD_ADD_MAC_ADDR_S			8
#define DPNI_CMD_REMOVE_MAC_ADDR_S		8
#define DPNI_CMD_CLR_MAC_TBL_S			0
#define DPNI_CMD_ADD_VLAN_ID_S			8
#define DPNI_CMD_REMOVE_VLAN_ID_S		8
#define DPNI_CMD_CLR_VLAN_TBL_S			0
#define DPNI_CMD_SET_QOS_TBL_S			8
#define DPNI_CMD_DELETE_QOS_TBL_S		0
#define DPNI_CMD_ADD_QOS_ENT_S			8
#define DPNI_CMD_REMOVE_QOS_ENT_S		0
#define DPNI_CMD_CLR_QOS_TBL_S			0
#define DPNI_CMD_SET_DIST_S			0
#define DPNI_CMD_SET_FS_TBL_S			8
#define DPNI_CMD_DELETE_FS_TBL_S		8
#define DPNI_CMD_ADD_FS_ENT_S			8
#define DPNI_CMD_REMOVE_FS_ENT_S		8
#define DPNI_CMD_CLR_FS_TBL_S			8
#define DPNI_CMD_GET_MFL_S			8
#define DPNI_CMD_GET_PRIM_MAC_S 		8

/* dpni_open */
/* param 1 */
#define DPNI_OPEN_ID_O				0
#define DPNI_OPEN_ID_S				16
#define DPNI_OPEN_ICID_O			16
#define DPNI_OPEN_ICID_S			16

/* dpni_init */
/* param 1*/
#define DPNI_INIT_MAX_TCS_O			0
#define DPNI_INIT_MAX_TCS_S			8
#define DPNI_INIT_TYPE_O			8
#define DPNI_INIT_TYPE_S			1
#define DPNI_INIT_DIST_KEY_VAR_O		9
#define DPNI_INIT_DIST_KEY_VAR_S		1
#define DPNI_INIT_MAC_ADDR_O			16
#define DPNI_INIT_MAC_ADDR_S			8	
/* param 2*/
#define DPNI_INIT_OPTIONS_O			0
#define DPNI_INIT_OPTIONS_S			32
#define DPNI_INIT_MAX_DPIOS_O 		32
#define DPNI_INIT_MAX_DPIOS_S		8
/*param 3*/
#define DPNI_INIT_MAX_DIST_O			0
#define DPNI_INIT_MAX_DIST_S			8
/* param 4 */
#define DPNI_INIT_MAX_UNICST_FILTR_O		0
#define DPNI_INIT_MAX_UNICST_FILTR_S		8
#define DPNI_INIT_MAX_MLTICST_FILTR_O		8
#define DPNI_INIT_MAX_MLTICST_FILTR_S		8
#define DPNI_INIT_MAX_VLAN_FILTR_O		16
#define DPNI_INIT_MAX_VLAN_FILTR_S		8
#define DPNI_INIT_MAX_QOS_ENTR_O		24
#define DPNI_INIT_MAX_QOS_ENTR_S		8
#define DPNI_INIT_MAX_QOS_KEY_SIZE_O		32
#define DPNI_INIT_MAX_QOS_KEY_SIZE_S		16
#define DPNI_INIT_MAX_DIST_KEY_SIZE_O		48
#define DPNI_INIT_MAX_DIST_KEY_SIZE_S		16

/* dpni_attach */
/* param 2 */
#define DPNI_ATTACH_DPIO_ID_O			0
#define DPNI_ATTACH_DPIO_ID_S			16
#define DPNI_ATTACH_DPSP_ID_O			16
#define DPNI_ATTACH_DPSP_ID_S			16
#define DPNI_ATTACH_STASH_EN_O			32
#define DPNI_ATTACH_STASH_EN_S			1
#define DPNI_ATTACH_DAN_EN_O			33
#define DPNI_ATTACH_DAN_EN_S			1
#define DPNI_ATTACH_FRAME_ANNO_O		40
#define DPNI_ATTACH_FRAME_ANNO_S		8
#define DPNI_ATTACH_FRAME_DATA_O		48
#define DPNI_ATTACH_FRAME_DATA_S		8
#define DPNI_ATTACH_FLOW_CTX_S_O		56
#define DPNI_ATTACH_FLOW_CTX_S_S		8
/* param 2 */
#define DPNI_ATTACH_RX_USR_CTX_O		0
#define DPNI_ATTACH_RX_USR_CTX_S		64
/* param 3 */
#define DPNI_ATTACH_RX_ERR_USR_CTX_O		0
#define DPNI_ATTACH_RX_ERR_USR_CTX_S		64
/* param 4 */
#define DPNI_ATTACH_TX_USR_CTX_O		0
#define DPNI_ATTACH_TX_USR_CTX_S		64
/* param 5 */
#define DPNI_ATTACH_TX_CONF_USR_CTX_O		0
#define DPNI_ATTACH_TX_CONF_USR_CTX_S		64
/*param 6 */
#define DPNI_ATTACH_ODP_O			0
#define DPNI_ATTACH_ODP_S			64
/* or */
#define DPNI_ATTACH_FLOW_CTX_ADDR_O		0
#define DPNI_ATTACH_FLOW_CTX_ADDR_S		64

/* dpni_attach_link */
/*param 1 */
#define DPNI_ATTACH_LINK_OBJ_TYPE_O		0
#define DPNI_ATTACH_LINK_OBJ_TYPE_S		3
#define DPNI_ATTACH_LINK_OBJ_ID_O		16
#define DPNI_ATTACH_LINK_OBJ_ID_S		16
#define DPNI_ATTACH_LINK_IF_ID_O		32
#define DPNI_ATTACH_LINK_IF_ID_S		8
/* dpni_set_tx_tc */
/* param 1 */
#define DPNI_SET_TX_TC_TCID_O			16
#define DPNI_SET_TX_TC_TCID_S			8

/* dpni_set_rx_tc */
/* param 1 */
#define DPNI_SET_RX_TC_DIST_O			0
#define DPNI_SET_RX_TC_DIST_S			8
#define DPNI_SET_RX_TC_TCID_O			16
#define DPNI_SET_RX_TC_TCID_S			8

/*dpni_set_tx_queue_cfg */
/* param 1 */
#define DPNI_SET_TX_Q_CFG_DPIO_O		0
#define DPNI_SET_TX_Q_CFG_DPIO_S		16
#define DPNI_SET_TX_Q_CFG_TCID_O		16
#define DPNI_SET_TX_Q_CFG_TCID_S		8
#define DPNI_SET_TX_Q_CFG_TX_CONF_O		24
#define DPNI_SET_TX_Q_CFG_TX_CONF_S		7
#define DPNI_SET_TX_Q_CFG_CKSUM_GEN_O		31
#define DPNI_SET_TX_Q_CFG_CKSUM_GEN_S		1
#define DPNI_SET_TX_Q_CFG_DEPTH_O		32
#define DPNI_SET_TX_Q_CFG_DEPTH_S		16
#define DPNI_SET_TX_Q_CFG_DAN_EN_O		48
#define DPNI_SET_TX_Q_CFG_DAN_EN_S		1
#define DPNI_SET_TX_Q_CFG_FETCH_REJ_O		49
#define DPNI_SET_TX_Q_CFG_FETCH_REJ_S		1
/* param 2 */
#define DPNI_SET_TX_Q_CFG_USR_CTX_O		0
#define DPNI_SET_TX_Q_CFG_USR_CTX_S		64
/* param 3 */
#define DPNI_SET_TX_Q_CFG_MOD_OPT_O		0
#define DPNI_SET_TX_Q_CFG_MOD_OPT_S		32

/* dpni_set_rx_flow_io */
/* param 1 */
#define DPNI_SET_RX_FLOW_IO_DPIO_O		33
#define DPNI_SET_RX_FLOW_IO_DPIO_S		1
#define DPNI_SET_RX_FLOW_IO_FLOWID_O		0
#define DPNI_SET_RX_FLOW_IO_FLOWID_S		16
#define DPNI_SET_RX_FLOW_IO_TCID_O		16
#define DPNI_SET_RX_FLOW_IO_TCID_S		8
#define DPNI_SET_RX_FLOW_IO_OPTIONS_O		32
#define DPNI_SET_RX_FLOW_IO_OPTIONS_S		32



/* dpni_set_rx_flow_cfg */
/* param 1 */
#define DPNI_SET_RX_FLOW_CFG_DAN_EN_O		33
#define DPNI_SET_RX_FLOW_CFG_DAN_EN_S		1
#define DPNI_SET_RX_FLOW_CFG_FLOWID_O		0
#define DPNI_SET_RX_FLOW_CFG_FLOWID_S		16
#define DPNI_SET_RX_FLOW_CFG_TCID_O		16
#define DPNI_SET_RX_FLOW_CFG_TCID_S		8
#define DPNI_SET_RX_FLOW_CFG_OPTIONS_O		32
#define DPNI_SET_RX_FLOW_CFG_OPTIONS_S		32
/*param 2 */
#define DPNI_SET_RX_FLOW_CFG_USR_CTX_O		0
#define DPNI_SET_RX_FLOW_CFG_USR_CTX_S		64


/* dpni_get_qdid */
/* param 1 */
#define DPNI_GET_QDID_O				0
#define DPNI_GET_QDID_S				16

/* dpni_get_tx_data_offset */
/* param 1 */
#define DPNI_GET_TX_DATA_OFF_O			0
#define DPNI_GET_TX_DATA_OFF_S			16

/* dpni_get_attributes */
/* param 1 */
#define DPNI_GET_ATTR_MAX_TCS_O			0
#define DPNI_GET_ATTR_MAX_TCS_S			8
/* param 2 */
#define DPNI_GET_ATTR_OPTIONS_O			0
#define DPNI_GET_ATTR_OPTIONS_S			64
/* param 3 */
#define DPNI_GET_ATTR_MAX_DIST_O		0
#define DPNI_GET_ATTR_MAX_DIST_S		8

/* dpni_set_mfl */
/* param 1 */
#define DPNI_SET_MFL_MFL_O			0
#define DPNI_SET_MFL_MFL_S			16

/* dpni_get_mfl */
/* param 1 */
#define DPNI_GET_MFL_O				0
#define DPNI_GET_MFL_S				8

/* dpni_set_mtu */
/* param 1 */
#define DPNI_SET_MTU_MTU_O			0
#define DPNI_SET_MTU_MTU_S			16

/* dpni_en_mcast_promisc */
/* param 1 */
#define DPNI_EN_MCAST_EN_O			0
#define DPNI_EN_MCAST_EN_S			1

/* dpni_set_primary_mac_addr */
/* param 1 */
#define DPNI_SET_PRIM_MAC_ADDR_O		16
#define DPNI_SET_PRIM_MAC_ADDR_S		8

/* dpni_get_primary_mac_addr */
/* param 1 */
#define DPNI_GET_PRIM_MAC_ADDR_O		16
#define DPNI_GET_PRIM_MAC_ADDR_S		8

/* dpni_add_mac_addr */
/* param 1 */
#define DPNI_ADD_MAC_ADDR_O			16
#define DPNI_ADD_MAC_ADDR_S			8

/* dpni_remove_mac_addr */
/* param 1 */
#define DPNI_REMOVE_MAC_ADDR_O			16
#define DPNI_REMOVE_MAC_ADDR_S			8

/* dpni_add_vlan_id */
/* param 1 */
#define DPNI_ADD_VLAN_ID_VID_O			32
#define DPNI_ADD_VLAN_ID_VID_S			12

/* dpni_remove_vlan_id */
/* param 1 */
#define DPNI_REMOVE_VLAN_ID_VID_O		32
#define DPNI_REMOVE_VLAN_ID_VID_S		12

/* dpni_set_qos_table */
/* param 1 */
#define DPNI_SET_QOS_TBL_NUM_ENT_O		0    /* TODO - exists? */
#define DPNI_SET_QOS_TBL_NUM_ENT_S		8
#define DPNI_SET_QOS_TBL_DROP_FRAME_O		8
#define DPNI_SET_QOS_TBL_DROP_FRAME_S		8
#define DPNI_SET_QOS_TBL_DEFAULT_TC_O		16
#define DPNI_SET_QOS_TBL_DEFAULT_TC_S		8

/* dpni_add_qos_entry */
/* param 1 */
#define DPNI_ADD_QOS_ENT_TCID_O			16
#define DPNI_ADD_QOS_ENT_TCID_S			8

/* dpni_set_dist */
/* param 1 */
#define DPNI_SET_DIST_DIST_FS_O			0
#define DPNI_SET_DIST_DIST_FS_S			1

/* dpni_set_fs_table */
/* param 1 */
#define DPNI_SET_FS_TBL_DEFAULT_FLOWID_O 	0
#define DPNI_SET_FS_TBL_DEFAULT_FLOWID_S 	8
#define DPNI_SET_FS_TBL_DROP_FRAME_O		8
#define DPNI_SET_FS_TBL_DROP_FRAME_S		8
#define DPNI_SET_FS_TBL_TCID_O			16
#define DPNI_SET_FS_TBL_TCID_S			8

/* dpni_delete_fs_table */
#define DPNI_DELETE_FS_TBL_TCID_O		16
#define DPNI_DELETE_FS_TBL_TCID_S		8

/* dpni_add_fs_entry */
#define DPNI_ADD_FS_ENT_FLOWID_O		0
#define DPNI_ADD_FS_ENT_FLOWID_S		16
#define DPNI_ADD_FS_ENT_TCID_O			16
#define DPNI_ADD_FS_ENT_TCID_S			8
/* TODO */

/* dpni_remove_fs_entry */
#define DPNI_REMOVE_FS_ENT_TCID_O		16
#define DPNI_REMOVE_FS_ENT_TCID_S		8
/* TODO */

/* dpni_clear_fs_table */
#define DPNI_CLR_FS_TBL_TCID_O			16
#define DPNI_CLR_FS_TBL_TCID_S			8

/* dpni_get_stats */
#define DPNI_GET_STAT_ADDR_O			0
#define DPNI_GET_STAT_ADDR_S			32


#endif /* _FSL_DPSW_CMD_H */
