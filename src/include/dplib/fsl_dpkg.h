#ifndef __FSL_DPKG_H_
#define __FSL_DPKG_H_

#include "common/types.h"
#include "net/fsl_net.h"

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

struct dpkg_init;
struct dpkg_init * dpkg_init(int ctluid);


struct dpkg_profile;
/**************************************************************************//**
 @Function      dpkg_profile_create

 @Description   Initialize a new Key Generation profile

 @Param[in]     profile_id 		- id of the profile
 @Param[in]     profile_params 	- A structure of parameters for creating a new Key Generation profile
 @Param[out]    keysize 		- The size (in bytes) of the key created
 
 @Return        A handle to the initialized Key extraction profile

 *//***************************************************************************/
struct dpkg_profile*    dpkg_profile_create(struct dpkg_init * dpkg_prf_mng,
                int profile_id, 
            struct dpkg_profile_params *params);

/**************************************************************************//**
 @Function      dpkg_profile_delete

 @Description   Deletes an existing Key Extraction Profile

 @Param[in]     dpkg_profile	- A handle to the initialized Key Profile
 
 @Return        0 on Success; error code otherwise.

 *//***************************************************************************/
int dpkg_profile_delete(struct dpkg_profile* dpkg_profile);


#endif /* __FSL_DPKG_H_ */
