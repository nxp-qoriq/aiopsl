/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


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
