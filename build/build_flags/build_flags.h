/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

#ifndef __BUILD_FLAGS_H
#define __BUILD_FLAGS_H


#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL         0
#endif /* DEBUG_LEVEL */

#ifndef DEBUG_ERRORS
#define DEBUG_ERRORS        1
#endif

#define DEBUG_GLOBAL_LEVEL  (REPORT_LEVEL_WARNING + DEBUG_LEVEL)
#define ERROR_GLOBAL_LEVEL  (REPORT_LEVEL_MAJOR + DEBUG_LEVEL)

#ifndef STACK_OVERFLOW_DETECTION
#define STACK_OVERFLOW_DETECTION 1
#endif

#if (DEBUG_LEVEL > 0)
#define DEBUG
#define DEBUG_FSL_MALLOC

#else
#define DISABLE_ASSERTIONS
#endif /* (DEBUG_LEVEL > 0) */

#define AIOP
#define SOC_PPC_CORE
/*#define SYS_SMP_SUPPORT*/
//#define SIMULATOR
#define DEBUG_NO_MC

/*
 * Code placement for various modules. Possible values are __COLD_CODE,
 * __WARM_CODE or __HOT_CODE.
 */
/*
 * IP Fragmentation module
 */ 
#ifndef IPF_CODE_PLACEMENT
#define IPF_CODE_PLACEMENT     __COLD_CODE
#endif
/*
 * IP Reassembly module
 */ 
#ifndef IPR_CODE_PLACEMENT
#define IPR_CODE_PLACEMENT     __COLD_CODE
#endif
/*
 * CAPWAP Reassembly module
 */
#ifndef CWAPR_CODE_PLACEMENT
#define CWAPR_CODE_PLACEMENT   __COLD_CODE
#endif
/*
 * CAPWAP Fragmentation module
 */
#ifndef CWF_CODE_PLACEMENT
#define CWF_CODE_PLACEMENT     __COLD_CODE
#endif
/*
 * TCP Reassembly module
 */ 
#ifndef GRO_CODE_PLACEMENT
#define GRO_CODE_PLACEMENT     __COLD_CODE
#endif
/*
 * TCP Segmentation module
 */ 
#ifndef GSO_CODE_PLACEMENT
#define GSO_CODE_PLACEMENT     __COLD_CODE
#endif
/*
 * RCU synchronization module
 */ 
#ifndef RCU_CODE_PLACEMENT
#define RCU_CODE_PLACEMENT     __COLD_CODE
#endif
/*
 * Slab allocator module
 */ 
#ifndef SLAB_CODE_PLACEMENT
#define SLAB_CODE_PLACEMENT    __COLD_CODE
#endif
/*
 * IPSEC module
 */ 
#ifndef IPSEC_CODE_PLACEMENT
#define IPSEC_CODE_PLACEMENT   __COLD_CODE
#endif
/*
 * Memory management module
 */ 
#ifndef MEM_MNG_CODE_PLACEMENT
#define MEM_MNG_CODE_PLACEMENT __COLD_CODE
#endif
/*
 * Time functions module
 */ 
#ifndef TIME_CODE_PLACEMENT
#define TIME_CODE_PLACEMENT    __COLD_CODE
#endif
/*
 * Random number generator module
 */ 
#ifndef RAND_CODE_PLACEMENT
#define RAND_CODE_PLACEMENT    __COLD_CODE
#endif
/*
 * Event manager module
 */ 
#ifndef EV_MNG_CODE_PLACEMENT
#define EV_MNG_CODE_PLACEMENT  __COLD_CODE
#endif
/*
 * Command interface module
 */ 
#ifndef CMDIF_CODE_PLACEMENT
#define CMDIF_CODE_PLACEMENT   __COLD_CODE
#endif
/*
 * Shared buffer pools module
 */ 
#ifndef SHBP_CODE_PLACEMENT
#define SHBP_CODE_PLACEMENT   __COLD_CODE
#endif
/*
 * TMAN module
 */ 
#ifndef TMAN_CODE_PLACEMENT
#define TMAN_CODE_PLACEMENT   __COLD_CODE
#endif
/*
 * Frame Operations module
 */ 
#ifndef FRAME_OP_CODE_PLACEMENT
#define FRAME_OP_CODE_PLACEMENT   __COLD_CODE
#endif
/*
 * HM module
 */ 
#ifndef HM_CODE_PLACEMENT
#define HM_CODE_PLACEMENT   __COLD_CODE
#endif
/*
 * Checksum module
 */ 
#ifndef CHECKSUM_CODE_PLACEMENT
#define CHECKSUM_CODE_PLACEMENT   __COLD_CODE
#endif

/*
 * IP Reassembly module
 * IPR_SW_TABLE
 *
 * #define USE_IPR_SW_TABLE
 *
 * It enables IPR to use software hash tables instead of CTLU ones.
 * Disabled by default.
 */


/*
 * sNIC module
 *
 * #define ENABLE_SNIC
 *
 * It enables/disables the sNIC module
 * It initializes the IPR, IPF, IPSec modules even when it's not used.
 * Disabled by default.
 */

#endif /* __BUILD_FLAGS_H */
