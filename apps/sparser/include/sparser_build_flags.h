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

/*******************************************************************************
 *                              - CAUTION -
 *
 * This code must not be distributed till we haven't a clear understanding
 * about what internals of the Parser may be exposed to customers.
 ******************************************************************************/

#ifndef __SPARSER_BUILD_FLAGS_H
#define __SPARSER_BUILD_FLAGS_H

/******************************************************************************/
/* When SP_DPAA_VERSION is 1 it is supposed the SP code contains DPAA1 specific
 * instructions. Use it if you want to disassemble DPAA1 soft parsers. DPAA1
 * SP behavior is partially simulated. The following DPAA1 specific opcodes are
 * not implemented or implemented as NOPs :
 *	1:Confirm_Layer_Mask
 *	3:OR_IV_LCV
 *	12:Load_LCV_to_WR
 *	13:Store_WR_to_LCV
 *
 * Usually SP_DPAA_VERSION is 2.
 *
 * WARNING : Unexpected behavior occurs if DPAA1 soft parser are loaded and
 * enabled on DPAA2 platforms.
 */
#define SP_DPAA_VERSION		2

#endif /* __SPARSER_BUILD_FLAGS_H */
