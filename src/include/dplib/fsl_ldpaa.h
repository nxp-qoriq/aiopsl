/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_ldpaa.h

 @Description   This file contains LDPAA general definitions.
 *//***************************************************************************/
#ifndef __FSL_LDPAA_H
#define __FSL_LDPAA_H

#include "common/types.h"


/**************************************************************************//**
 @Group         ldpaa_g  LDPAA API

 @Description   TODO

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Description	FLC for stashing
*//****************************************************************************/
struct ldpaa_flc_stashing {
	uint8_t frame_annotation_size;
	/**< Size of Frame  Annotation to be stashed */
	uint8_t frame_data_size;
	/**< Size of Frame Data to be stashed. */
	uint8_t flow_context_size;
	/**< Size of flow context to be stashed. */
	uint64_t flow_context_addr;
/**< 64/49 bit memory address containing the
 flow context information to be stashed;
 Must be cacheline-aligned */
};

/**************************************************************************//**
 @Description	FLC for ODP
 *//***************************************************************************/
typedef uint64_t ldpaa_flc_odp_t;

/**************************************************************************//**
 @Description   structure representing flow context
 *//***************************************************************************/
struct ldpaa_flow_ctx {
	int stash_en;
	union {
		struct ldpaa_flc_stashing stash;
		ldpaa_flc_odp_t odp;
	} u;
};

/**************************************************************************//**
 @Description	Frame Descriptor structure.

 The Frame Descriptor (FD) includes information related to the
 frame, such as frame format, the amount of frame data, presence
 of an annotation section containing frame meta-data.
 *//***************************************************************************/
struct ldpaa_fd {
	uint64_t addr; /**< Buffer address 	 */
	uint32_t length; /**< Frame data length 	 */
	uint32_t offset; /**< Frame data offset 	 */
	uint32_t frc; /**< Frame Context 	 */
	uint32_t control; /**< Fame control fields */
	uint64_t flc; /**< Flow Context 	 */
};

/* FD fields Getters and Setters */

#define LDPAA_FD_GET_ADDR(_fd)
/**< Macro to get FD ADDRESS field.*/
/* Todo - 64/49 bit address. Phys to Virt? */
#define LDPAA_FD_SET_ADDR(_fd,_val)
/**< Macro to set FD ADDRESS field */
/* Todo - 64/49 bit address. Phys to Virt? */
#define LDPAA_FD_GET_LENGTH(_fd)
/**< Macro to get FD LENGTH field */
#define LDPAA_FD_SET_LENGTH(_fd,_val)
/**< Macro to set FD LENGTH field */
#define LDPAA_FD_GET_MEM(_fd)
/**< Macro to get FD MEM field */
#define LDPAA_FD_SET_MEM(_fd,_val)
/**< Macro to set FD MEM field */
#define LDPAA_FD_GET_BPID(_fd)
/**< Macro to get FD BPID field */
#define LDPAA_FD_SET_BPID(_fd,_val)
/**< Macro to set FD BPID field */
#define LDPAA_FD_GET_IVP(_fd)
/**< Macro to get FD IVP field */
#define LDPAA_FD_SET_IVP(_fd,_val)
/**< Macro to set FD IVP field */
#define LDPAA_FD_GET_BMT(_fd)
/**< Macro to get FD BMT field */
#define LDPAA_FD_SET_BMT(_fd,_val)
/**< Macro to set FD BMT field */
#define LDPAA_FD_GET_OFFSET(_fd)
/**< Macro to get FD OFFSET field */
#define LDPAA_FD_SET_OFFSET(_fd,_val)
/**< Macro to set FD OFFSET field */
#define LDPAA_FD_GET_FMT(_fd)
/**< Macro to get FD FMT field */
#define LDPAA_FD_SET_FMT(_fd,_val)
/**< Macro to set FD FMT field */
#define LDPAA_FD_GET_SL(_fd)
/**< Macro to get FD SL field */
#define LDPAA_FD_SET_SL(_fd,_val)
/**< Macro to set FD SL field */
#define LDPAA_FD_GET_FRC(_fd)
/**< Macro to get FD FRC field */
#define LDPAA_FD_SET_FRC(_fd,_val)
/**< Macro to set FD FRC field */
#define LDPAA_FD_GET_ERR(_fd)
/**< Macro to get FD ERR field */
#define LDPAA_FD_SET_ERR(_fd,_val)
/**< Macro to set FD ERR field */
#define LDPAA_FD_GET_VA(_fd)
/**< Macro to get FD VA field */
#define LDPAA_FD_SET_VA(_fd,_val)
/**< Macro to set FD VA field */
#define LDPAA_FD_GET_CBMT(_fd)
/**< Macro to get FD CBMT field */
#define LDPAA_FD_SET_CBMT(_fd,_val)
/**< Macro to set FD CBMT field */
#define LDPAA_FD_GET_ASAL(_fd)
/**< Macro to get FD ASAL field */
#define LDPAA_FD_SET_ASAL(_fd,_val)
/**< Macro to set FD ASAL field */
#define LDPAA_FD_GET_PTV2(_fd)
/**< Macro to get FD PTV2 field */
#define LDPAA_FD_SET_PTV2(_fd,_val)
/**< Macro to set FD PTV2 field */
#define LDPAA_FD_GET_PTV1(_fd)
/**< Macro to get FD PTV1 field */
#define LDPAA_FD_SET_PTV1(_fd,_val)
/**< Macro to set FD PTV1 field */
#define LDPAA_FD_GET_PTA(_fd)
/**< Macro to get FD PTA field */
#define LDPAA_FD_SET_PTA(_fd,_val)
/**< Macro to set FD PTA field */
#define LDPAA_FD_GET_DROPP(_fd)
/**< Macro to get FD DROPP field */
#define LDPAA_FD_SET_DROPP(_fd,_val)
/**< Macro to set FD DROPP field */
#define LDPAA_FD_GET_SC(_fd)
/**< Macro to get FD SC field */
#define LDPAA_FD_SET_SC(_fd,_val)
/**< Macro to set FD SC field */
#define LDPAA_FD_GET_DD(_fd)
/**< Macro to get FD DD field */
#define LDPAA_FD_SET_DD(_fd,_val)
/**< Macro to set FD DD field */
#define LDPAA_FD_GET_CS(_fd)
/**< Macro to get FD CS field */
#define LDPAA_FD_SET_CS(_fd,_val)
/**< Macro to set FD CS field */
#define LDPAA_FD_GET_AS(_fd)
/**< Macro to get FD AS field */
#define LDPAA_FD_SET_AS(_fd,_val)
/**< Macro to set FD AS field */
#define LDPAA_FD_GET_DS(_fd)
/**< Macro to get FD DS field */
#define LDPAA_FD_SET_DS(_fd,_val)
/**< Macro to set FD DS field */
#define LDPAA_FD_GET_FLC(_fd)
/**< Macro to get FD FLC field */
#define LDPAA_FD_SET_FLC(_fd,_val)
/**< Macro to set FD FLC field */

/** @} *//* end of ldpaa_g group */

#endif /* __FSL_LDPAA_H */
