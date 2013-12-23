/**************************************************************************//*
 @File          fsl_dprc_cmd.h

 @Description   defines dprc portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPRC_CMD_H
#define _FSL_DPRC_CMD_H

/* cmd IDs */
#define DPRC_CMD_OPEN				0x150
#define DPRC_CMD_CLOSE				0x151

#define DPRC_CMD_CREATE_CONT			0x152
#define DPRC_CMD_DESTROY_CONT			0x153
#define DPRC_CMD_GET_CONT_ID			0x154
#define DPRC_CMD_RST_CONT			0x155
#define DPRC_CMD_SET_RES_ALLOC_P		0x156
#define DPRC_CMD_GET_RES_ALLOC_P		0x157

#define DPRC_CMD_ASSIGN				0x159

#define DPRC_CMD_UNASSIGN			0x15B

#define DPRC_CMD_GET_DEV_COUNT			0x15D
#define DPRC_CMD_GET_DEVICE			0x15E
#define DPRC_CMD_GET_RES_COUNT			0x15F
#define DPRC_CMD_GET_RES_IDS			0x160
#define DPRC_CMD_GET_ATTR			0x161
#define DPRC_CMD_GET_DEV_REG			0x162
#define DPRC_CMD_SET_IRQ			0x163
#define DPRC_CMD_GET_IRQ			0x164

/* cmd sizes */
#define DPRC_CMD_OPEN_S				8
#define DPRC_CMD_CLOSE_S			0

#define DPRC_CMD_CREATE_CONT_S			(8 * 2)
#define DPRC_CMD_DESTROY_CONT_S			8
#define DPRC_CMD_GET_CONT_ID_S			8
#define DPRC_CMD_RST_CONT_S			8
#define DPRC_CMD_SET_RES_ALLOC_P_S		8

#define DPRC_CMD_ASSIGN_S			(8 * 2)
#define	DPRC_CMD_SET_UNASSIGN_PORTAL_P_S 	8
#define DPRC_CMD_UNASSIGN_S			(8 * 2)
//#define DPRC_CMD_UNASSIGN_EXPLICIT_S		8
#define DPRC_CMD_GET_DEV_COUNT_S		0
#define DPRC_CMD_GET_DEVICE_S			(8 * 3)
#define DPRC_CMD_GET_RES_COUNT_S		8
#define DPRC_CMD_GET_RES_IDS_S			8
#define DPRC_CMD_GET_ATTR_S			(8 * 2)
#define DPRC_CMD_GET_DEV_REG_S			(8 * 2)
#define DPRC_CMD_SET_IRQ_S			(8 * 2)
#define DPRC_CMD_GET_IRQ_S			(8 * 2)

/* dprc_open */
/* param 1 */
#define DPRC_OPEN_ID_O				0
#define DPRC_OPEN_ID_S				16
#define DPRC_OPEN_ICID_O			16
#define	DPRC_OPEN_ICID_S			16

/* dprc_get_container_id */
/* param 1 */
#define DPRC_GET_CONT_ID_O			0
#define DPRC_GET_CONT_ID_S			32
#define	DPRC_GET_CONT_ID_PORTAL_O		32
#define DPRC_GET_CONT_ID_PORTAL_S		32

/* dprc_create_container */
/* param 1 */
#define DPRC_CREATE_CONT_CHILD_ID_O		0
#define DPRC_CREATE_CONT_CHILD_ID_S		32
#define DPRC_CREATE_CONT_ICID_O			32
#define DPRC_CREATE_CONT_ICID_S			16
#define DPRC_CREATE_CONT_SPAWN_O		48		
#define DPRC_CREATE_CONT_SPAWN_S		1
#define DPRC_CREATE_CONT_ALLOC_O		49
#define DPRC_CREATE_CONT_ALLOC_S		1
#define DPRC_CREATE_CONT_ID_O			56
#define DPRC_CREATE_CONT_ID_S			8
/* param 2 */
#define DPRC_CREATE_CONT_OPTIONS_O		0
#define DPRC_CREATE_CONT_OPTIONS_S		32
#define DPRC_CREATE_CONT_CHILD_PORTAL_O		32
#define DPRC_CREATE_CONT_CHILD_PORTAL_S		32

/* dprc_destroy_container */
/* param 1 */
#define DPRC_DESTROY_CONT_ID_O			0	
#define DPRC_DESTROY_CONT_ID_S			32

/* dprc_reset_container */
/* param 1 */
#define DPRC_RST_CONT_ID_O			0
#define DPRC_RST_CONT_ID_S			32

/* dprc_set_res_alloc_policy */
/* param 1 */
#define DPRC_SET_RES_ALLOC_P_CONT_ID_O		0
#define DPRC_SET_RES_ALLOC_P_CONT_ID_S		32
#define DPRC_SET_RES_ALLOC_P_RES_TYPE_O		32
#define DPRC_SET_RES_ALLOC_P_RES_TYPE_S		6
#define DPRC_SET_RES_ALLOC_P_O			38
#define DPRC_SET_RES_ALLOC_P_S			1
#define DPRC_SET_RES_ALLOC_P_QUOTA_O		48
#define DPRC_SET_RES_ALLOC_P_QUOTA_S		16

/* dprc_get_res_alloc_policy */
/* param 1 */
#define DPRC_GET_RES_ALLOC_P_CONT_ID_O		0
#define DPRC_GET_RES_ALLOC_P_CONT_ID_S		32
#define DPRC_GET_RES_ALLOC_P_RES_TYPE_O		32
#define DPRC_GET_RES_ALLOC_P_RES_TYPE_S		6
#define DPRC_GET_RES_ALLOC_P_O			38
#define DPRC_GET_RES_ALLOC_P_S			1
#define DPRC_GET_RES_ALLOC_P_QUOTA_O		48
#define DPRC_GET_RES_ALLOC_P_QUOTA_S		16

/* dprc_assign */
/* param 1 */
#define DPRC_ASSIGN_CONT_ID_O			0
#define DPRC_ASSIGN_CONT_ID_S			32
#define DPRC_ASSIGN_RES_TYPE_O			32
#define DPRC_ASSIGN_RES_TYPE_S			6
#define DPRC_ASSIGN_ALIGN_O			38
#define DPRC_ASSIGN_ALIGN_S			1
#define DPRC_ASSIGN_NUM_OF_RES_O		41
#define DPRC_ASSIGN_NUM_OF_RES_S		7
/* param 2 */
#define DPRC_ASSIGN_OPTIONS_O			0
#define DPRC_ASSIGN_OPTIONS_S			32

/* dprc_unassign */
/* param 1 */
#define DPRC_UNASSIGN_CONT_ID_O			0
#define DPRC_UNASSIGN_CONT_ID_S			32
#define DPRC_UNASSIGN_RES_TYPE_O		32
#define DPRC_UNASSIGN_RES_TYPE_S		6
#define DPRC_UNASSIGN_ALIGN_O			38
#define DPRC_UNASSIGN_ALIGN_S			1
#define DPRC_UNASSIGN_SEQ_O			39
#define DPRC_UNASSIGN_SEQ_S			1
#define DPRC_UNASSIGN_VIRT_O			40
#define DPRC_UNASSIGN_VIRT_S			16
#define DPRC_UNASSIGN_NUM_OF_RES_O		41
#define DPRC_UNASSIGN_NUM_OF_RES_S		7
/* param 2 */
#define DPRC_UNASSIGN_OPTIONS_O			0
#define DPRC_UNASSIGN_OPTIONS_S			32

/* dprc_get_dev_count */
/* param 1 */
#define DPRC_GET_DEV_COUNT_COUNT_O		32
#define DPRC_GET_DEV_COUNT_COUNT_S		8

/* dprc_get_devices */
/* param 1 */
#define DPRC_GET_DEV_REV_MINOR_O		0
#define DPRC_GET_DEV_REV_MINOR_S		8
#define DPRC_GET_DEV_REV_MAJOR_O		8
#define DPRC_GET_DEV_REV_MAJOR_S		8
#define DPRC_GET_DEV_TYPE_O			16
#define DPRC_GET_DEV_TYPE_S			16
#define DPRC_GET_DEV_ID_O			32	
#define DPRC_GET_DEV_ID_S			32
/* param 2 */
#define DPRC_GET_DEV_VENDOR_O			0
#define DPRC_GET_DEV_VENDOR_S			16
#define DPRC_GET_DEV_IRQS_O			16
#define DPRC_GET_DEV_IRQS_S			8
#define DPRC_GET_DEV_REGIONS_O			24
#define DPRC_GET_DEV_REGIONS_S			8
#define DPRC_GET_DEV_STATE_O			32
#define DPRC_GET_DEV_STATE_S			32
/* param 3 */
#define DPRC_GET_DEV_IDX_O			0
#define DPRC_GET_DEV_IDX_S			16 

/* dprc_get_res_count */
/* param 1 */
#define DPRC_GET_RES_COUNT_COUNT_O		32
#define DPRC_GET_RES_COUNT_COUNT_S		6
#define DPRC_GET_RES_COUNT_TYPE_O		24	
#define DPRC_GET_RES_COUNT_TYPE_S		8

/*	dprc_get_res_ids */
/* param 1 */
#define DPRC_GET_RES_IDS_RES_IDS_O		0
#define DPRC_GET_RES_IDS_RES_IDS_S		32
#define DPRC_GET_RES_IDS_RES_TYPE_O		32
#define DPRC_GET_RES_IDS_RES_TYPE_S		6
#define DPRC_GET_RES_IDS_NUM_OF_IDS_O		41
#define DPRC_GET_RES_IDS_NUM_OF_IDS_S		7
#define DPRC_GET_RES_IDS_IDX_O			48
#define DPRC_GET_RES_IDS_IDX_S			8

/* dprc_set_unassign_portal_policy */
/* param 1 */
#define DPRC_SET_UNASSIGN_PORTAL_P_O		0	
#define DPRC_SET_UNASSIGN_PORTAL_P_S		1

/* dprc_get_attributes */
/* param 1 */
#define DPRC_GET_ATTR_CONT_ID_O			0
#define DPRC_GET_ATTR_CONT_ID_S			32
#define DPRC_GET_ATTR_ICID_O			32
#define DPRC_GET_ATTR_ICID_S			16
#define DPRC_GET_ATTR_PORTAL_O			48
#define DPRC_GET_ATTR_PORTAL_S			16
/* param 2 */
#define DPRC_GET_ATTR_OPTIONS_O			0
#define DPRC_GET_ATTR_OPTIONS_S			32
#define DPRC_GET_ATTR_SPAWN_O			32
#define DPRC_GET_ATTR_SPAWN_S			1
#define DPRC_GET_ATTR_ALLOC_O			33
#define DPRC_GET_ATTR_ALLOC_S			2

/* dprc_get_dev_region */
/* param 1 */
#define DPRC_GET_DEV_REG_DEV_ID_O		0
#define DPRC_GET_DEV_REG_DEV_ID_S		16
#define DPRC_GET_DEV_REG_DEV_TYPE_O		16
#define DPRC_GET_DEV_REG_DEV_TYPE_S		16
#define DPRC_GET_DEV_REG_IDX_O			32
#define DPRC_GET_DEV_REG_IDX_S			8
#define DPRC_GET_DEV_REG_SIZE_O			48
#define DPRC_GET_DEV_REG_SIZE_S			16
/* param 2 */
#define DPRC_GET_DEV_REG_BASE_PADDR_O		0
#define DPRC_GET_DEV_REG_BASE_PADDR_S		64

/* dprc_set_irq */
/* param 1 */
#define DPRC_SET_IRQ_VAL_O			0
#define DPRC_SET_IRQ_VAL_S			32
#define DPRC_SET_IRQ_IDX_O			32
#define DPRC_SET_IRQ_IDX_S			8
/*param 2 */
#define DPRC_SET_IRQ_PADDR_O			0
#define DPRC_SET_IRQ_PADDR_S			64

/* dprc_get_irq */
/* param 1 */
#define DPRC_GET_IRQ_VAL_O			0
#define DPRC_GET_IRQ_VAL_S			32
#define DPRC_GET_IRQ_IDX_O			32
#define DPRC_GET_IRQ_IDX_S			8
/*param 2 */
#define DPRC_GET_IRQ_PADDR_O			0
#define DPRC_GET_IRQ_PADDR_S			64

#endif /* _FSL_DPRC_CMD_H */
