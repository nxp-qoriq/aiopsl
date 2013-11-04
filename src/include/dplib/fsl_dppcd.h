#ifndef __FSL_DPPCD_H_
#define __FSL_DPPCD_H_

#include "common/types.h"
#include "net/fsl_net.h"


/* Used by MC only */
extern struct dppcd *dppcd;

enum dppcd_ctlu_id {
	DPPCD_CTLU_0, /* WRIOP */
	DPPCD_CTLU_1, /* AIOP */
	DPPCD_CTLU_2, /* WRIOP */
	DPPCD_CTLU_3, /* WRIOP */
	DPPCD_CTLU_4,
/* AIOP */
};

struct dppcd_init_params {
	enum dppcd_ctlu_id ctlu_id;
	void *cmd_if; /* IFP regs or AIOP CI */
};

struct dppcd *dppcd_init(struct dppcd_init_params *dppcd_init_params);


/***********************************************************/
/***********************************************************/
/********              Policy                     **********/
/***********************************************************/
/***********************************************************/

#define DPPOLICY_MAX_NUM_OF_CLASSIFIERS				    8
#define DPPOLICY_RULE_MAX_NUM_OF_IDENTIFIERS			64

/**************************************************************************//**
 @Description    DP Action Type

 Enumeration type defining the flow by selecting the next 
 action under certain conditions
 
 *//***************************************************************************/
enum dppcd_action_type {
	DPPCD_ACTION_CONTINUE_LOOKUP,
	DPPCD_ACTION_FORWARD,
	DPPCD_ACTION_REPLICATE,
	DPPCD_ACTION_DISCARD
};

/**************************************************************************//**
 @Description    DP Action Parameters

 *//***************************************************************************/
/* TBD */
struct dppcd_action {
	enum dppcd_action_type type;
	uint32_t override_fields_flags; /**< one or more flags out of: DPPCD_... */
	uint32_t qdid;
	uint16_t ifp_id;
	fsl_handle_t dpplcr_id;
	uint8_t qpri;
	uint8_t hash_dpkg_profile_id;
	union {
		struct {
			fsl_handle_t dptbl;
			fsl_handle_t dpkg_profile;
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
 @Description   Enumeration type for selecting the table result 
 *//***************************************************************************/
enum dppcd_result_type {
	DPPCD_RESULT_TYPE_ACTION, DPPCD_RESULT_TYPE_DATA
};

/**************************************************************************//**
 @Description   Enumeration type for selecting timestamp 
 *//***************************************************************************/
/* TBD */
enum dptbl_monitor {
	DPTBL_MONITOR_NONE = 0,
	DPTBL_MONITOR_STATISTICS,
	DPTBL_MONITOR_TIMESTAMP,
	DPTBL_MONITOR_AGING
};
/**************************************************************************//**
 @Description    DP Result parameters

 Defines the parameters for the next action
 
 *//***************************************************************************/
/* TBD */
struct dppcd_result_cfg {
	enum dppcd_result_type type;/**< data, action */
	union {
		struct dppcd_action action;
		uint64_t *data;
	} result;
	enum dptbl_monitor monitor;
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
                      struct dppcd_action *action);

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
                         struct dppcd_action *new_action);

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

/***********************************************************/
/***********************************************************/
/********              Key Generation             **********/
/***********************************************************/
/***********************************************************/
#define DPKG_MAX_NUM_OF_EXTRACTS_PER_KEY			64
#define DPKG_NUM_OF_MASKS					4
#define DPKG_MAX_NUM_OF_EXTRACTS				8

/**************************************************************************//**
 @Description   Enumeration type for selecting extraction by header types
 *//***************************************************************************/
enum dpkg_extract_from_hdr_type {
	DPKG_FROM_HDR, /**< Extract bytes from header *//* TBD - Name Used below!!! */
	DPKG_FROM_FIELD, /**< Extract bytes from header field */
	DPKG_FULL_FIELD
/**< Extract a full field */
};

/**************************************************************************//**
 @Description   Enumeration type for selecting extraction source
 (when it is not DPKG_EXTRACT_FROM_CONTEXT )
 *//***************************************************************************/
enum dpkg_extract_from_context_type {
	DPKG_FROM_PARSE_RESULT, /**< Extract from the parser result */
	DPKG_FROM_FCV,
/**< Extract from enqueue FQID */
/* add logic context's */
};

/**************************************************************************//**
 @Description   Enumeration type for selecting extraction type
 *//***************************************************************************/
enum dpkg_extract_type {
	DPKG_EXTRACT_FROM_HDR, /**< Extract according to header *//* TBD - Name aleady used !!! */
	DPKG_EXTRACT_FROM_DATA, /**< Extract from data that is not the header
	 */
	DPKG_EXTRACT_FROM_CONTEXT, /**< Extract from data that is not the header
	 */
	DPKG_EXTRACT_CONSTANT
/**< Extract private info as specified by user
 */
};

/**************************************************************************//**
 @Description   A structure for defining a single extraction
 *//***************************************************************************/
struct dpkg_mask {
	uint8_t mask;
	uint8_t offset;
};

struct dpkg_extract {
	enum dpkg_extract_type type;
	/**< Type may be one of the members of
	 enum: dpkg_extract_type; 
	 type determines how the union
	 below will be interpretted:
	 DPKG_EXTRACT_FROM_HDR: select "from_hdr";
	 DPKG_EXTRACT_FROM_DATA: select 
	 "from_data";
	 DPKG_EXTRACT_FROM_CONTEXT: select
	 "from_context";
	 DPKG_EXTRACT_CONSTANT: select constant. */

	union {
		struct {
			enum net_prot prot;
			/**< One of the headers supported by CTLU */
			uint8_t hdr_index;
			/**< Clear for cases not listed below;
			 Used for protocols that may have more
			 than a single header, 0 indicates 
			 outer header; 
			 Supported protocols (possible values):
			 NET_PROT_VLAN (0, HDR_INDEX_LAST);
			 NET_PROT_MPLS (0, 1, HDR_INDEX_LAST);
			 NET_PROT_IP(0, HDR_INDEX_LAST);
			 NET_PROT_IPv4(0, HDR_INDEX_LAST);
			 NET_PROT_IPv6(0, HDR_INDEX_LAST);
			 */

			enum dpkg_extract_from_hdr_type type;
			/**< Defines the type of header extraction:     
			 DPKG_FROM_HDR: use size & offset
			 below;
			 DPKG_FROM_FIELD: use field, size
			 & offset below;
			 DPKG_FULL_FIELD: use field below 
			 */

			net_hdr_fld_t field;
			/**< One of the supported fields. */
			uint8_t size; /**< Size in byte */
			uint8_t offset; /**< Byte offset */
		} from_hdr; /**< used when type = DPKG_EXTRACT_FROM_HDR */
		struct {
			uint8_t size; /**< Size in byte */
			uint8_t offset; /**< Byte offset */
		} from_data; /**< used when type = DPKG_EXTRACT_FROM_DATA */
		struct {
			enum dpkg_extract_from_context_type src;
			/**< Non-header extraction source */
			uint8_t size; /**< Size in byte */
			uint8_t offset; /**< Byte offset */
		} from_context; /**< used when type = DPKG_EXTRACT_FROM_CONTEXT */
		struct {
			uint8_t constant; /**< a constant value */
			uint8_t num_of_repeats;
		/**< The number of times 
		 the constant is to be
		 entered to the key */
		} constant; /**< used when type = DPKG_EXTRACT_CONSTANT */
	} extract;
	uint8_t num_of_byte_masks;
	/**< Defines the valid number of entries in the
	 array below; This is also number of bytes
	 to be used as masks */
	struct dpkg_mask masks[DPKG_NUM_OF_MASKS]; /**< Mask parameters */
};

/**************************************************************************//**
 @Description   A structure for defining a Key Extraction rule
 *//***************************************************************************/
struct dpkg_profile_params {
	uint8_t num_extracts; /**< defines the valid number of
	 entries in the array below */
	struct dpkg_extract extracts[DPKG_MAX_NUM_OF_EXTRACTS];
/**< An array of extractions definition. */
};

struct dpkg_profile;
/**************************************************************************//**
 @Function      dpkg_profile_create

 @Description   Initialize a new Key Generation profile

 @Param[in]     profile_id 		- id of the profile
 @Param[in]     profile_params 	- A structure of parameters for creating a new Key Generation profile
 @Param[out]    keysize 		- The size (in bytes) of the key created
 
 @Return        A handle to the initialized Key extraction profile

 *//***************************************************************************/
struct dpkg_profile* dpkg_profile_create(int profile_id,
                                         struct dpkg_profile_params *params,
                                         uint16_t *keysize);

/**************************************************************************//**
 @Function      dpkg_profile_delete

 @Description   Deletes an existing Key Extraction Profile

 @Param[in]     dpkg_profile	- A handle to the initialized Key Profile
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dpkg_profile_delete(struct dpkg_profile* dpkg_profile);

/***********************************************************/
/***********************************************************/
/********              Tables                     **********/
/***********************************************************/
/***********************************************************/

/**************************************************************************//**
 @Description   Enumeration type for selecting the lookup table type
 *//***************************************************************************/
enum dptbl_type {
	DPTBL_TYPE_EXACT_MATCH = 0, /**< Exact match table */
	DPTBL_TYPE_LPM, /**< Longest prefix match table */
	DPTBL_TYPE_TCAM_ACL, /**< TCAM ACL table */
	DPTBL_TYPE_ALG_ACL
/**< Algorithmic ACL table */
};

/**************************************************************************//**
 @Description   A structure for defining a key for a single rule 
 in the lookup table
 *//***************************************************************************/
struct dptbl_rule_cfg {
	union {
		struct {
			int keysize; /**< key size */
			void *key; /**< A pointer to the key */
		} exact; /**< To be used for exact match tables */
		struct {
			int priority; 
			/**< logical, relative to Policy; 0 is lowest;
			 This priority can not change during the
			 lifetime of a Policy. It is user
			 responsibility to space the priorities
			 according to consequent rule additions. */
			int keysize; /**< key size */
			void *key; /**< A pointer to the key */
			void *mask; /**< A pointer to the mask */
		} masked; /**< To be used for TBD tables */
		struct {
			int keysize; /**< key size */
			void *key; /**< A pointer to the key */
			int prefix_size;/**< The size of the prefix */
		} prefix; /**< To be used for longest-prefix tables */
		struct {
			int tmp;
		/*TBD */
		} range; /**< To be used for TBD tables */
	} rule_cfg;
};

/**************************************************************************//**
 @Description   A structure of parameters for creating a new lookup table
 *//***************************************************************************/
struct dptbl_init_params {
	enum dptbl_type type; /**< Match table type */
	int mem_type; /**< TBD */
	uint32_t max_rules; /**< Maximum number of rules for
	 the lifetime of this table. */
	uint8_t aging_threshold; /**< TBD */
	uint8_t key_size; /**< The size of the keys that will
	 occupy this table */
	struct dppcd_result_cfg *result_on_miss;/**< The result is case a "miss"
	 occures in this table. */
};

struct dptbl;
/*************************************************************************//**
 @Function      dptbl_create

 @Description   Initialize a new lookup table

 @Param[in]     table_init_params 	- A structure of parameters for creating a new lookup table
 
 @Return        0 on Success; error code otherwize.

 *//***************************************************************************/
struct dptbl* dptbl_create(struct dptbl_init_params *init_params);

/**************************************************************************//**
 @Function      dptbl_modify_miss_result

 @Description   Modifies the miss action for the specified table

 @Param[in]     dptbl 		   - A handle to the initialized table
 @Param[in]     action_on_miss - a pointer to a structure describing the new action
 
 @Return        0 on Success; error code otherwize.

 *//***************************************************************************/
int dptbl_modify_miss_result(struct dptbl* dptbl,
                             struct dppcd_result_cfg *result_on_miss);

/**************************************************************************//**
 @Function      dptbl_delete

 @Description   Initialize a new lookup table

 @Param[in]     dptbl 		- A handle to the initialized table
 
 @Return        0 on Success; error code otherwize.

 *//***************************************************************************/
int dptbl_delete(struct dptbl* dptbl);

/**************************************************************************//**
 @Function      dptbl_add_rule

 @Description   Initialize a new lookup table

 @Param[in]     dptbl 		- A handle to the initialized table
 @Param[in]     rule  		- A structure of parameters describing the rule.
 @Param[in]     result  	- A structure of parameters describing the result of this rule.
 
 @Return        0 on Success; error code otherwize.

 *//***************************************************************************/
int dptbl_add_rule(struct dptbl* dptbl,
                   struct dptbl_rule_cfg *rule,
                   struct dppcd_result_cfg *result);

/**************************************************************************//**
 @Function      dptbl_modify_rule

 @Description   Initialize a new lookup table

 @Param[in]     dptbl 		- A handle to the initialized table
 @Param[in]     rule  		- A structure of parameters describing the existing rule.
 @Param[in]     new_result  - A structure of parameters for describing the new result.
 
 @Return        0 on Success; error code otherwize.

 *//***************************************************************************/
int dptbl_modify_rule(struct dptbl* dptbl,
                      struct dptbl_rule_cfg *rule,
                      struct dppcd_result_cfg *new_result);

/**************************************************************************//**
 @Function      dptbl_remove_rule

 @Description   Deletes an existing lookup key

 @Param[in]     dptbl 		- A handle to the initialized table
 @Param[in]     rule  	    - A structure of parameters describing the rule to be deleted.
 
 @Return        0 on Success; error code otherwize.

 *//***************************************************************************/
int dptbl_remove_rule(struct dptbl* dptbl, struct dptbl_rule_cfg *rule);

/***********************************************************/
/***********************************************************/
/********              Parser                     **********/
/***********************************************************/
/***********************************************************/
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
#define EGRESS_FLOW      					0x00000010
/**< when not selected, this profile will
 be initialized as an ingress profile;
 Select to initialize an egress profile. */
/* @} */

/**************************************************************************//**
 @Description   Parser Profile Configuration Parameters
 This structure is used to hold the default Parser Profile parameters.
 *//***************************************************************************/
#define MAX_ALLOWED_HXS_DISABLE_REPORT 		8

struct dpprs_profile_cfg_params {
	enum net_prot disable_err_report[MAX_ALLOWED_HXS_DISABLE_REPORT];
	/**< Errors from the selected headers will 
	 not be reported in PHE;
	 Default: All set to HDR_NONE */

	uint16_t vlan_tpid[2]; /**< Configures a distinct Ethertype value
	 (or TPID value) to indicate a VLAN tag
	 in addition to the common TPID values
	 0x8100 and 0x88A8; 
	 Default: 0 */
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
	 after MPLS; Must be L3 and up;
	 Default: HDR_NONE */
	void *ifp_regs; /**< Optional:Private IFP regs pointer,
	 used for issuing management 
	 commands; if NULL common DP IFP
	 will be used. */

};

/**************************************************************************//**
 @Description   Parser Profile Initialization Parameters
 This structure is used to hold the MUST Parser Profile parameters.
 *//***************************************************************************/
struct dpprs_profile_init_params {
	int ctlu_id;
	enum net_prot start_hdr; /**< The header from which parser is to
	 start parsing */
	uint8_t start_offset; /**< The offset of start_hdr from the
	 beginning of the frame */
};

struct dpprs_profile;
/**************************************************************************//**
 @Function      dpprs_profile_init

 @Description   Initialize a new Parser Profile

 @Param[in]     params 	- A structure of parameters for creating a new Parser profile
 
 @Return        A handle to the initialized Parser profile

 *//***************************************************************************/
struct dpprs_profile *dpprs_profile_init(int profile_id,
                                         struct dpprs_profile_init_params *dpprs_profile_init,
                                         struct dpprs_profile_cfg_params *dpprs_profile_cfg);

/**************************************************************************//**
 @Function      dpprs_delete_profile

 @Description   Deletes an existing Parser Profile

 @Param[in]     dpprs_profile 		- A handle to the initialized Parser Profile
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dpprs_delete_profile(struct dpprs_profile* dpprs_profile);


#endif /* FSL_DPPCD_H_ */
