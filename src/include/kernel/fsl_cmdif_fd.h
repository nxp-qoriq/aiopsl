/*!
 *  @file    fsl_cmdif_fd.h
 *  @brief   Cmdif AIOP<->GPP FLIB header file describing frame descriptor
 */

#ifndef __FSL_CMDIF_FD_H
#define __FSL_CMDIF_FD_H


#include <types.h>

/**************************************************************************//**
@Group         cmdif_g  Command Interface API

@Description   AIOP and GPP command interface API

@{
 *//***************************************************************************/

/**************************************************************************//**
@Group         cmdif_flib_g  Command Interface - FLIB API

@Description   API to be used for FD based command interface implementation

@{
 *//***************************************************************************/

/*!
 * The following are the FD fields that are used by CMDIF
 * cmdif_fd.fls, cmdif_fd.frc, cmdif_fd.d_addr, cmdif_fd.d_size
 * should be copied into real FD */
struct cmdif_fd {
	/*! FD[FLC] Frame descriptor relevant fields as should be set
	 * by cmdif client side when sending commands to AIOP server */
	union {
		uint64_t flc;
		struct {
			uint8_t dev_h;     /*!< 7 high bits of cmdif_desc.dev */
			uint8_t err;       /*!< Reserved for error on response*/
			uint16_t auth_id;  /*!< Authentication id */
			uint16_t cmid;     /*!< Command id */
			uint16_t epid;     /*!< Reserved fog EPID */
		} cmd;
		struct {
			uint8_t inst_id;    /*!< Module instance id*/
			uint8_t reserved0;
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t epid;      /*!< Reserved fog EPID */
		} open;
		/*!< Open command is always synchronous */
		struct {
			uint8_t reserved[2];
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t epid;      /*!< Reserved fog EPID */
		} close;
		/*!< Close command is always synchronous*/
		uint32_t word[2];
	} u_flc;

	/*! FD[FRC] Frame descriptor relevant fields as should be set
	 * by cmdif client side when sending commands to AIOP server */
	union  {
		uint32_t frc;
		struct {
			uint32_t dev_l;   /*!< 32 low bit of cmdif_desc.dev */
		} cmd;
	} u_frc;

	uint32_t d_size; /*!< Data length */
	union {
		uint64_t d_addr; /*!< Data address */
		uint32_t word[2];
	}u_addr;
};

/** @} *//* end of cmdif_flib_g group */
/** @} *//* end of cmdif_g group */

#endif /* __FSL_CMDIF_FD_H */
