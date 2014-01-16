/**************************************************************************//*
 @File          fsl_dprc_cmd.h

 @Description   defines dprc portal commands

 @Cautions      None.
 *//***************************************************************************/

#ifndef _FSL_DPRC_CMD_H
#define _FSL_DPRC_CMD_H

/* Command IDs */
#define DPRC_CMDID_CREATE_CONT			0x151
#define DPRC_CMDID_DESTROY_CONT			0x152
#define DPRC_CMDID_GET_CONT_ID			0x830
#define DPRC_CMDID_RESET_CONT			0x154
#define DPRC_CMDID_SET_RES_QUOTA		0x155
#define DPRC_CMDID_GET_RES_QUOTA		0x156
#define DPRC_CMDID_ASSIGN			0x157
#define DPRC_CMDID_UNASSIGN			0x158
#define DPRC_CMDID_GET_DEV_COUNT		0x159
#define DPRC_CMDID_GET_DEVICE			0x15A
#define DPRC_CMDID_GET_RES_COUNT		0x15B
#define DPRC_CMDID_GET_RES_IDS			0x15C
#define DPRC_CMDID_GET_ATTR			0x15D
#define DPRC_CMDID_GET_DEV_REG			0x15E
#define DPRC_CMDID_SET_IRQ			0x15F
#define DPRC_CMDID_GET_IRQ			0x160

/* Command sizes */
#define DPRC_CMDSZ_CREATE_CONT			(8*2)
#define DPRC_CMDSZ_DESTROY_CONT			8
#define DPRC_CMDSZ_GET_CONT_ID			8
#define DPRC_CMDSZ_RESET_CONT			8
#define DPRC_CMDSZ_SET_RES_QUOTA		8
#define DPRC_CMDSZ_GET_RES_QUOTA		8
#define DPRC_CMDSZ_ASSIGN			(8*3)
#define DPRC_CMDSZ_UNASSIGN			(8*2)
#define DPRC_CMDSZ_GET_DEV_COUNT		0
#define DPRC_CMDSZ_GET_DEVICE			(8*3)
#define DPRC_CMDSZ_GET_RES_COUNT		8
#define DPRC_CMDSZ_GET_RES_IDS			8
#define DPRC_CMDSZ_GET_ATTR			(8*2)
#define DPRC_CMDSZ_GET_DEV_REG			(8*2)
#define DPRC_CMDSZ_SET_IRQ			(8*2)
#define DPRC_CMDSZ_GET_IRQ			(8*2)


/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_CONTAINER_ID(_OP) \
	_OP(0,  0, 	32, 	int,			container_id)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_CREATE_CONTAINER(_OP) \
	_OP(0, 	32, 	16, 	uint16_t, 		attr->icid) \
	_OP(0,  0, 		32, 	uint32_t,		attr->options) \

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_CREATE_CONTAINER(_OP) \
	_OP(1,  0, 	32, 	int, 			child_container_id) \
	_OP(2,  0, 	64, 	uint64_t,		child_portal_paddr)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_DESTROY_CONTAINER(_OP) \
	_OP(0, 	0, 	32, 	int, 			child_container_id)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_RESET_CONTAINER(_OP) \
	_OP(0, 	0, 	32, 	int, 			child_container_id)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_SET_RES_QUOTA(_OP) \
	_OP(0, 	0, 		32, 	int, 			child_container_id) \
	_OP(0, 	32,  	10, 	uint16_t, 		res_type_def) \
	_OP(0,  48, 	16, 	uint16_t,		quota)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_GET_RES_QUOTA(_OP) \
	_OP(0, 	0, 		32, 	int, 			child_container_id) \
	_OP(0, 	32,  	10, 	uint16_t, 		res_type_def)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_RES_QUOTA(_OP) \
	_OP(0, 	48, 	16, 	uint16_t, 			quota) 

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_ASSIGN(_OP) \
	_OP(0, 	0, 		32, 	int, 			container_id) \
	_OP(0, 	32,  	10, 	uint16_t, 		type_def) \
	_OP(1, 	0,  	32, 	uint32_t, 		res_req->num) \
	_OP(1, 	32,  	32, 	int, 			res_req->id_base_align) \
	_OP(2, 	0,  	32, 	uint32_t, 		res_req->options)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_UNASSIGN(_OP) \
	_OP(0, 	0, 		32, 	int, 			child_container_id) \
	_OP(0, 	32,  	10, 	uint16_t, 		type_def) \
	_OP(1, 	0,  	32, 	uint32_t, 		res_req->num) \
	_OP(1, 	32,  	32, 	int, 			res_req->id_base_align) \
	_OP(2, 	0,  	32, 	uint32_t, 		res_req->options)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_DEV_COUNT(_OP) \
	_OP(0, 	32, 	32, 	int,			dev_count)


/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_GET_DEVICE(_OP) \
	_OP(2, 	0, 	16, 	int,			dev_index)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_DEVICE(_OP) \
	_OP(0, 	0, 		8, 		uint8_t,		dev_desc->rev_minor) \
	_OP(0, 	8, 		8, 		uint8_t,		dev_desc->rev_major) \
	_OP(0, 	16, 	16, 	uint16_t,		type_def) \
	_OP(0, 	32, 	32, 	int,			dev_desc->id) \
	_OP(1, 	0, 		16, 	uint16_t,		dev_desc->vendor) \
	_OP(1, 	16, 	8, 		uint8_t,		dev_desc->irq_count) \
	_OP(1, 	24, 	8, 		uint8_t,		dev_desc->region_count) \
	_OP(1, 	32, 	32,	 	uint32_t,		dev_desc->state)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_GET_RES_COUNT(_OP) \
	_OP(0, 	32, 	10, 	uint16_t,		type_def)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_RES_COUNT(_OP) \
	_OP(0, 	0, 		32, 	int,			res_count)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_GET_RES_IDS(_OP) \
	_OP(0, 	32, 10,	uint16_t,			 res_type_def) \
	_OP(0, 	42, 7, 	enum dprc_rage_ineration_status, range_desc->interation_status)

//TODO fix this response
/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_RES_IDS(_OP) \
	_OP(0, 	42, 	7, 	enum dprc_rage_ineration_status, range_desc->interation_status)\
	_OP(1, 	0, 	32, 	int,			range_desc->base_id) \
	_OP(1, 	32, 	32, 	int,			range_desc->last_id) 

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_ATTRIBUTES(_OP) \
	_OP(0, 	0, 	32, 	int,			attr->container_id) \
	_OP(0, 	32, 	16, 	uint16_t,		attr->icid) \
	_OP(0, 	48, 	16, 	uint16_t,		attr->portal_id) \
	_OP(1, 	0, 	32, 	uint32_t,		attr->options)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_GET_DEV_REGION(_OP) \
	_OP(0, 	0, 		16, 	uint16_t,		dev_id) \
	_OP(0, 	16, 	16, 	uint16_t,		dev_type_def) \
	_OP(0, 	32, 	8, 		uint8_t,		region_index)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_DEV_REGION(_OP) \
	_OP(0, 	48, 	16, 	uint16_t,		region_desc->size) \
	_OP(1, 	0, 	64, 	uint64_t,		region_desc->base_paddr)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_SET_IRQ(_OP) \
	_OP(0, 	32, 	8, 		uint8_t,		irq_index) \
	_OP(0, 	0, 		32, 	uint32_t,		irq_val) \
	_OP(1, 	0, 		64, 	uint64_t,		irq_paddr)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_CMD_GET_IRQ(_OP) \
	_OP(0, 	32, 	8, 	uint8_t,		irq_index)

/* 	param, offset, width, 	type, 			arg_name */
#define DPRC_RSP_GET_IRQ(_OP) \
	_OP(0, 	0, 	32, 	uint32_t,		irq_val) \
	_OP(1, 	0, 	64, 	uint64_t,		irq_paddr)



#if 0
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
/*param 2*/
#define DPRC_ASSIGN_NUM_OF_RES_O		0
#define DPRC_ASSIGN_NUM_OF_RES_S		32
#define DPRC_ASSIGN_ALIGN_O			32
#define DPRC_ASSIGN_ALIGN_S			32
/* param 3 */
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

/* dprc_get_device */
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
#define DPRC_GET_RES_COUNT_COUNT_S		6		//not enough
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

#define DPRC_GET_RES_IDS_VALID_CNT_O		0
#define DPRC_GET_RES_IDS_VALID_CNT_S		32
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
#endif

#endif /* _FSL_DPRC_CMD_H */
