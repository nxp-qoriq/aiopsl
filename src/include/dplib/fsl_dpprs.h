/**************************************************************************//**
 @File          dpprs.h

 @Description   Parser Application Programming Interface.
*//***************************************************************************/
#ifndef __FSL_DPPRS_H
#define __FSL_DPPRS_H

#include "common/types.h"
#include "net/fsl_net.h"


/**************************************************************************//**

 @Group         DPPRS_grp Parser API

 @Description   Parser functions, definitions and enums

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         DPPRS_init_grp FM Port Initialization Unit

 @Description   Parser Initialization Unit

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Collection    Parser parameters options (prs_options)
*//***************************************************************************/
#define MARK_ICMPV6_ATOMIC_FRAGS     	0x01   /**< Select to mark ICMPv6 atomic
 	 	 	 	 	 	 	 	 	 	 	 	 	fragments as fragments. */
#define CLK_POWER_DOWN_ENABLE			0x02   /**< Select to allow various portions
                                                    of the block to be powered down. */
/* @} */


/**************************************************************************//**
 @Description   Parser Configuration Parameters
                This structure is used to hold the default Parser parameters.
*//***************************************************************************/
struct dpprs_cfg_params {
	uint16_t 	cycle_limit;			  /**< Maximum Frame Parsing cycle limit;
										   When this value is exceeded, the Frame
										   Parsing ceases parsing on the frame and
										   report the error as an exception 
										   notification; A limit value of 0 disables
										   this mechanism. Default = 256 TBD */
	uint8_t		dpprs_options; 		      /**< OR'ed options:
	                                           MARK_ICMPV6_ATOMIC_FRAGS
	                                           CLK_POWER_DOWN_ENABLE */
};

/**************************************************************************//**
 @Function      dpprs_defconfig

 @Description   This function obtains Parser default parameters.

			    Note: This function should be called only once
			    during the lifetime of the object 
				
 @Param[out]    cfg_params   - Default Parser parameters

 @Retval        None.
*//***************************************************************************/
void dpprs_defconfig(struct dpprs_cfg_params *cfg_params);

/**************************************************************************//**
 @Function      dpprs_init

 @Description  This function initializes the Parser module hardware registers.

			   Note: This function should be called only once
		       during the lifetime of the object 

 @Param[in]    cfg_params   - Default Parser parameters as returned by 
                               dpprs_defconfig and possibly modified by user 
							   prior to this routine's call

 @Retval        None.
*//***************************************************************************/
void dpprs_init(void *regs, struct dpprs_cfg_params *cfg_params);


/** @} */ /* end of DPPRS_init_grp group */

/**************************************************************************//**
 @Group         DPPRS_runtime_grp FM Port Initialization Unit

 @Description   Parser Runtime Unit

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      dpprs_enable

 @Description   This function enables the global parser.

 @Retval        None.
*//***************************************************************************/
void dpprs_enable(void *regs);

/**************************************************************************//**
 @Function      dpprs_disable

 @Description   This function disables the global parser.

 @Retval        None.
*//***************************************************************************/
void dpprs_disable(void *regs);


/** @} */ /* end of DPPRS_runtime_grp group */
/** @} */ /* end of DPPRS_grp group */

#endif /* _FSL_DPPRS_H */
