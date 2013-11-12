#ifndef FSL_DPPOLICY_H_
#define FSL_DPPOLICY_H_

#include "common/types.h"
#include "net/fsl_net.h"

#define DPPOLICY_MAX_NUM_OF_CLASSIFIERS				    8
#define DPPOLICY_RULE_MAX_NUM_OF_IDENTIFIERS			64

/**************************************************************************//**
 @Description    Policy Action Type

 Enumeration type defining the flow by selecting the next 
 action under certain conditions
 
 *//***************************************************************************/
/* TODO check for Policy*/
enum dppolicy_action_type {
	DPPOLICY_ACTION_CONTINUE_LOOKUP,
	DPPOLICY_ACTION_FORWARD,
	DPPOLICY_ACTION_REPLICATE,
	DPPOLICY_ACTION_DISCARD
};

/**************************************************************************//**
 @Description    DP Action Parameters

 *//***************************************************************************/
/* TBD */
struct dppolicy_action {
	enum dppolicy_action_type type;
	uint32_t override_fields_flags; /**< one or more flags out of: DPPCD_... */
	uint32_t qdid;
	uint16_t ifp_id;
	fsl_handle_t dpplcr_id;
	uint8_t qpri;
	uint8_t hash_dpkg_profile_id;
	union {
		struct {
			void *dptbl;
			void *dpkg_profile;
			/*int						override_fields_flags;*/
			uint16_t replic_id;
		} continue_lookup;
		struct {
			uint16_t replic_id;
		} replic;
		struct {
			uint16_t flc;
		} forward;
	} action;
};


/**************************************************************************//**
 @Description    Policy identifier

 Defines the possible types of Policy identifier
 
 *//***************************************************************************/
enum dppolicy_identifier_type {
	DPPOLICY_IDENTIFIER_PROT, /**< The identifier is a protocol */
	DPPOLICY_IDENTIFIER_PROT_OPT,
	/**< The identifier is a protocol option */
	DPPOLICY_IDENTIFIER_PROT_PRS_ERR,
	/**< The identifier is a specific protocol 
	 parse error */
	DPPOLICY_IDENTIFIER_GEN_PRS_ERR
/**< The identifier is a general protocol 
 parse error (any error) */

};

/**************************************************************************//**
 @Description    Policy rule identifier

 This structure hold the parameters needed to define a single
 classification rule.
 
 *//***************************************************************************/
struct identifier{
	int exclude;
	/**< 0 to allow the identifier described below,
	 1 to reject. */
	enum dppolicy_identifier_type type;
	/**< Type of identifier,
	 also determines which of the following
	 parameters should be initialized:
	 DPPOLICY_IDENTIFIER_PROT: set "prot" and
	 "hdr_index" when applicable;
	 DPPOLICY_IDENTIFIER_PROT_OPT: set 
	 "prot", "prot_opt" and "hdr_index" when
	 applicable;
	 DPPOLICY_IDENTIFIER_PROT_PRS_ERR: set
	 "prot" and "hdr_index" when applicable;
	 DPPOLICY_IDENTIFIER_GEN_PRS_ERR: No 
	 parameters required. */
	enum net_prot prot;
	/**< One of the headers supported by CTLU;
	 Must be set for all "type"s but
	 DPPOLICY_IDENTIFIER_GEN_PRS_ERR*/
	net_hdr_option_t opt;
	/**< Option must match prot;
	 Should only be set for
	 DPPOLICY_IDENTIFIER_PROT_OPT*/
	uint8_t hdr_index;
/**< Clear for cases not listed below;
 Used for protocols that may have more
 than a single header, 0 indicates 
 outer header; 
 Supported protocols (possible values):
 NET_PROT_VLAN (0, HDR_INDEX_LAST);
 NET_PROT_MPLS (0, HDR_INDEX_LAST);
 NET_PROT_IP(0, HDR_INDEX_LAST);
 NET_PROT_IPv4(0, HDR_INDEX_LAST);
 NET_PROT_IPv6(0, HDR_INDEX_LAST);
 */
};

/**************************************************************************//**
 @Description    Policy rule identifier

 This structure hold the parameters needed to define a single
 classification rule.
 
 *//***************************************************************************/
struct dppolicy_rule_cfg {
	int priority;
	/**< logical, relative to Policy; 0 is lowest;
	 This priority can not change during the
	 lifetime of a Policy. It is user
	 responsibility to space the priorities
	 according to consequent rule additions. */
	int num_of_identifiers;
	/**< Number of identifiers, defines the size
	 of the array below */
	struct identifier identifiers[DPPOLICY_RULE_MAX_NUM_OF_IDENTIFIERS];
};

/**************************************************************************//**
 @Description    Policy Init Parameters

 This structure hold the parameters needed to define a Policy
 
 *//***************************************************************************/
struct dppolicy_params {
	uint8_t ctlu_id; /**< TBD */
	uint16_t max_num_of_rules; /**< The total number of
	 classification rules at any
	 given time during the lifetime
	 of the Policy. */
};

struct dppolicy_mng;
/**************************************************************************//**
 @Function      dppolicy_mng_init

 @Description   
 Note:
 
 
 @Retval        
 *//***************************************************************************/

struct dppolicy_mng*  dppolicy_mng_init();
/**************************************************************************//**
 @Function      dppolicy_mng_init

 @Description   

 Note: 
 
 
 @Retval        A handle to the parser memory.
 *//***************************************************************************/
int dppolicy_mng_delete(struct dppolicy_mng* dppolicy_mng);



struct dppolicy;
/**************************************************************************//**
 @Function      dppolicy_create

 @Description   Create a new empty Policy

 @Param[in]     dppolicy_id 	- id of the Policy
 @Param[in]     params 			- A structure of parameters for creating a new Policy
 
 @Return       TBD

 *//***************************************************************************/
struct dppolicy* dppolicy_create(int dppolicy_id,
                                 struct dppolicy_params *params);

/**************************************************************************//**
 @Function      dppolicy_delete

 @Description   Deletes an existing Policy

 @Param[in]     dppolicy 		- A handle to the initialized Policy
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dppolicy_delete(struct dppolicy* dppolicy);

/**************************************************************************//**
 @Function      dppolicy_add_rule

 @Description   

 @Param[in]     dppolicy 		- A handle to the initialized Policy
 @Param[in]     rule 	        - A structure of parameters for describing
 the required classifier
 @Param[in]     action 	    	- A structure of parameters for describing
 the action to be executed for this rule
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dppolicy_add_rule(struct dppolicy* dppolicy,
                      struct dppolicy_rule_cfg *rule,
                      struct dppolicy_action *action);

/**************************************************************************//**
 @Function      dppolicy_modify_rule

 @Description   Change the action of the specified rule

 @Param[in]     dppolicy 		- A handle to the initialized Policy
 @Param[in]     rule 	        - A structure of parameters for describing
 the required classifier
 @Param[in]     new_action 	    - A structure of parameters for describing
 the new action to replace the current one
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dppolicy_modify_rule(struct dppolicy *dppolicy,
                         struct dppolicy_rule_cfg *rule,
                         struct dppolicy_action *new_action);

/**************************************************************************//**
 @Function      dppolicy_remove_rule

 @Description   

 @Param[in]     dppolicy 		- A handle to the initialized Policy
 @Param[in]     rule 	        - A structure of parameters for describing
 the required classifier
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dppolicy_remove_rule(struct dppolicy *dppolicy,
                         struct dppolicy_rule_cfg *rule);


#endif /* FSL_DPPOLICY_H_ */
