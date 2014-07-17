
#ifndef __DESC_COMMON_H__
#define __DESC_COMMON_H__

/**
 * @file                 common.h
 * @brief                SEC Descriptor Construction Library Protocol-level
 *                       Shared Descriptor Constructors - shared structures
 */

/**
 * @defgroup descriptor_lib_group RTA Descriptors Library
 * @{
 */
/** @} end of descriptor_lib_group */

/**
 * @defgroup typedefs_group Auxiliary Data Structures
 * @ingroup descriptor_lib_group
 * @{
 */

/**
 * @struct    alginfo common.h
 * @details   Container for IPsec algorithm details
 */
struct alginfo {
	uint32_t algtype;  /**< Algorithm selector. For valid values, see
				documentation of the functions where it is
				used */
	uint64_t key;      /**< Address where algorithm key resides */
	uint32_t keylen;   /**< Length of the provided key, in bytes */
	uint32_t key_enc_flags; /**< Key encryption flags */
};

/**
 * @struct    protcmd common.h
 * @details   Container for Protocol Operation Command fields.
 */
struct protcmd {
	uint32_t optype;    /**< Command type. */
	uint32_t protid;    /**< Protocol Identifier */
	uint16_t protinfo;  /**< Protocol Information. */
};

/** @} */ /* end of typedefs_group */

#endif /* __DESC_ALGO_H__ */
