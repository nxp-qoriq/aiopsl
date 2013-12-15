/**************************************************************************//**
@File		cdma.h

@Description	This file contains the AIOP SW CDMA internal API
*//***************************************************************************/


#ifndef __CDMA_H_
#define __CDMA_H_

#include "common/types.h"
#include "dplib/fsl_cdma.h"


/* CDMA Command IDs */
	/** CDMA Acquire Context Memory command code */
#define CDMA_ACQUIRE_CONTEXT_MEM_CMD	0x00000080
	/** CDMA Access Context command code */
#define CDMA_ACCESS_CONTEXT_MEM_CMD	0x00000081
	/** CDMA Release Context Memory command code  */
#define CDMA_RELEASE_CONTEXT_MEM_CMD	0x00000082
	/** CDMA Memory Initialization command code */
#define CDMA_MEMORY_INIT_CMD		0x00000083

/* CDMA Commands general definitions */
	/** CDMA Status Mask. */
#define CDMA_STATUS_MASK		0x000000FF
	/** CDMA Status Offset. */
#define CDMA_STATUS_OFFSET		0x3
	/** CDMA Status Offset. */
#define CDMA_REF_CNT_OFFSET		0x4

/* CDMA Command Flags */
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,00,0] */
#define CDMA_READ_CMD_FLAGS  0x00000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,00,0] */
#define CDMA_WRITE_CMD_FLAGS  0x00000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[DA]=[01] */
#define CDMA_READ_CMD_ENB    0x40000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[DA]=[10] */
#define CDMA_WRITE_CMD_ENB    0x80000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,01,0] */
#define CDMA_REFCNT_INC_CMD_FLAGS  0x00000200
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,11,0] */
#define CDMA_REFCNT_DEC_CMD_FLAGS  0x00000600
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,10,0] */
#define CDMA_REFCNT_DEC_REL_CMD_FLAGS  0x00000400
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,00,1] */
#define CDMA_EXT_MUTEX_LOCK_RELEASE_CMD_FLAGS  0x00000100
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,10,01,0] */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_FLAGS  0x00001200
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,11,1] */
#define CDMA_WRITE_REL_LOCK_AND_DEC_CMD_FLAGS  0x00000700


/* CDMA Command Arguments */
#define CDMA_ACQUIRE_CONTEXT_MEM_CMD_ARG2(_ws_return_address, _pool_id)	\
	(uint32_t)((_ws_return_address << 16) | _pool_id)
#define CDMA_READ_CMD_ARG1()	\
	(CDMA_READ_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_READ_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_READ_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_WRITE_CMD_ARG1()	\
	(CDMA_WRITE_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_WRITE_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_EXT_MUTEX_LOCK_TAKE_CMD_ARG1(_flags)	\
	(_flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_EXT_MUTEX_LOCK_RELEASE_CMD_ARG1()	\
	(CDMA_EXT_MUTEX_LOCK_RELEASE_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_READ_WITH_MUTEX_CMD_ARG1(_flags)	\
	(_flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_READ_WITH_MUTEX_CMD_ARG2(_size, _ws_address) \
	(uint32_t)((((_size << 16) | CDMA_READ_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_WRITE_WITH_MUTEX_CMD_ARG1(_flags)	\
	(_flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_WITH_MUTEX_CMD_ARG2(_size, _ws_address) \
	(uint32_t)((((_size << 16) | CDMA_WRITE_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_REFCNT_INC_CMD_ARG1()	\
	(CDMA_REFCNT_INC_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_REFCNT_DEC_CMD_ARG1()	\
	(CDMA_REFCNT_DEC_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_REFCNT_DEC_REL_CMD_ARG1()	\
	(CDMA_REFCNT_DEC_REL_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_ARG1()	\
	(CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_FLAGS |	\
	CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_READ_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_WRITE_REL_LOCK_AND_DEC_CMD_ARG1()	\
	(CDMA_WRITE_REL_LOCK_AND_DEC_CMD_FLAGS |	\
	CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_REL_LOCK_AND_DEC_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_WRITE_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_MEMORY_INIT_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)(((_size << 16) & 0xFFFF0000) | (_ws_address & 0x0000FFFF))
#define CDMA_ACCESS_CONTEXT_MEM_CMD_ARG1(_ext_offset, _flags)	\
	((_ext_offset << 16) | _flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_ACCESS_CONTEXT_MEM_CMD_ARG2(_dma_param, _ws_address) \
	(uint32_t)((_dma_param << 16) | (_ws_address & 0x0000FFFF))


#endif /* __CDMA_H_ */
