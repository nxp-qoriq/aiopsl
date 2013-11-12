/**************************************************************************//**
 @File          dpparser.h

 @Description   Parser Application Programming Interface.
 *//***************************************************************************/
#ifndef __FSL_DPPARSER_H
#define __FSL_DPPARSER_H

#include "common/types.h"
#include "net/fsl_net.h"
#include "fsl_ctlu.h"

/**************************************************************************//**

 @Group         DPPARSER_grp Parser API

 @Description   Parser functions, definitions and enums

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Group         DPPARSER_init_grp FM Port Initialization Unit

 @Description   Parser Initialization Unit

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Collection    Parser parameters options (prs_options)
 *//***************************************************************************/
#define MARK_ICMPV6_ATOMIC_FRAGS	0x01   
/**< Select to mark ICMPv6 atomic
 fragments as fragments. */
#define CLK_POWER_DOWN_ENABLE		0x02   
/**< Select to allow various portions
 of the block to be powered down. */
/* @} */

/**************************************************************************//**
 @Description   Parser Configuration Parameters
 This structure is used to hold the default Parser parameters.
 *//***************************************************************************/
struct dpparser_cfg {
	struct ctlu *ctlu;
	uint16_t cycle_limit; /**< Maximum Frame Parsing cycle limit;
	 When this value is exceeded, the Frame
	 Parsing ceases parsing on the frame and
	 report the error as an exception 
	 notification; A limit value of 0 disables
	 this mechanism. */
	uint8_t dpparser_options; /**< OR'ed options:
	 MARK_ICMPV6_ATOMIC_FRAGS
	 CLK_POWER_DOWN_ENABLE */
};

/**************************************************************************//**
 @Function      dpparser_alloc

 @Description   This function obtains the memory resources needed for the parser object.

 Note: This function should be called only once
 during the lifetime of the object 
 
 @Retval        A handle to the parser memory.
 *//***************************************************************************/
struct dpparser *dpparser_alloc(void);

/**************************************************************************//**
 @Function      dpparser_init

 @Description  This function initializes the Parser module hardware registers.

 Note: This function should be called only once
 during the lifetime of the object 

 @Param[in]    cfg_params   - Default Parser parameters as returned by 
 dpparser_defconfig and possibly modified by user 
 prior to this routine's call

 @Retval        None.
 *//***************************************************************************/
int dpparser_init(struct dpparser *dpparser, void *regs, struct dpparser_cfg *cfg);

/** @} *//* end of DPPARSER_init_grp group */

/**************************************************************************//**
 @Group         DPPARSER_runtime_grp FM Port Initialization Unit

 @Description   Parser Runtime Unit

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Collection    Parser parameters options (prs_options)
 *//***************************************************************************/
#define PPP_DISABLE_MTU_CHECK     	       	0x00000001 
/**< When selected, MTU is not checked;
 Ethernet has a maximum payload size
 of 1500 octets; PPPoE header is 6
 octets and the PPP Protocol ID is
 2 octets thus the PPP MTU can not
 exceed 14921; (RFC2516 section 7) */
#define IPV6_ROUTE_HDR_ENABLE            	0x00000002
/**< When not selected the routing header
 is ignored and the destination
 address from the Main header is used
 instead; The presence of the routing
 header is still reported in the Parse
 Array but is not handed off with 
 the Parse Result. */
#define L4_IGNORE_PAD_FOR_CHECKSUM  	   	0x00000004   
/**< Relevant for TCP/UDP; When selected,
 THE padded region at the end of a
 frame is removed from the checksum
 calculation; Required for layer 4
 checksum validation. */
#define MPLS_LABEL_INTERPRETATION_DISABLE	0x00000008   
/**< When not selected, the last MPLS label
 is interpreted as defined below:
 MPLS Label 0 --> IPv4
 MPLS Label 1 --> Other L3 Shell
 MPLS Label 2 --> IPv6
 MPLS Label 3-15 --> Other L3 Shell
 MPLS Label > 15 --> Default next 
 parse sequence;
 When selected, the Frame Parsing
 advances to MPLS default_next_parse
 which must be properly configured
 */
/* @} */

/**************************************************************************//**
 @Description   Parser Profile Configuration Parameters
 This structure is used to hold the default Parser Profile parameters.
 *//***************************************************************************/
#define MAX_ALLOWED_HXS_DISABLE_REPORT 		8

struct dpparser_profile_cfg {
	enum net_prot start_hdr; /**< The header from which parser is to
	 start parsing */
	uint8_t start_offset; /**< The offset of start_hdr from the
	 beginning of the frame */
	struct {
		enum net_prot disable_err_report[MAX_ALLOWED_HXS_DISABLE_REPORT];
		/**< Errors from the selected headers will 
		 not be reported in PHE; */
		uint16_t vlan_tpid[2]; /**< Configures a distinct Ethertype value
		 (or TPID value) to indicate a VLAN tag
		 in addition to the common TPID values
		 0x8100 and 0x88A8 */
		uint32_t prs_profile_options;
		/**< OR'ed options:
		 PPP_DISABLE_MTU_CHECK
		 IPV6_ROUTE_HDR_ENABLE
		 L4_IGNORE_PAD_FOR_CHECKSUM
		 MPLS_LABEL_INTERPRETATION_DISABLE
		 EGRESS_FLOW */
		enum net_prot mpls_default_next_parse;
		/**< Relevant when mpls_label_interpret_enable
		 is cleared; Next header to be parsed 
		 after MPLS; Must be L3 and up; */
#if 0		
		void *cmd_if; /**< Private command interface pointer,
		 used for issuing commands; if uninitialized, common CTLU 
		 interface will be used. */
#endif		
	} optional_cfg;

};

/**************************************************************************//**
 @Function      dpparser_init_profile

 @Description   Initialize a new Parser Profile

 @Param[in]     params 	- A structure of parameters for creating a new Parser profile
 
 @Return        A handle to the initialized Parser profile

 *//***************************************************************************/
int dpparser_init_profile(struct dpparser *dpparser,
	int profile_id,
	struct dpparser_profile_cfg *cfg);

/**************************************************************************//**
 @Function      dpparser_delete_profile

 @Description   Deletes an existing Parser Profile

 @Param[in]     dpparser_profile 		- A handle to the initialized Parser Profile
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dpparser_delete_profile(struct dpparser *dpparser, int profile_id);

/** @} *//* end of DPPARSER_runtime_grp group */
/** @} *//* end of DPPARSER_grp group */

#endif /* _FSL_DPPARSER_H */
