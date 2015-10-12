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
/******************************************************************************
 @File          log.h

 @Description   LOG internal structures and definitions.
 *//***************************************************************************/

#ifndef __LOG_H
#define __LOG_H

#include "fsl_types.h"
#include <stdio.h>
#include "fsl_icontext.h"
#include "fsl_aiop_common.h"
#include "fsl_log.h"
#include "fsl_cdma.h"
#include "fsl_spinlock.h"
#include "fsl_malloc.h"

struct log_header {
	char magic_word[8]; /* magic word 'AIOP<version>' */
	uint32_t buf_start; /* holds the 32-bit little-endian offset of the start of the buffer */
	uint32_t buf_length; /* holds the 32-bit little-endian length of the buffer - not including these initial words */
	uint32_t last_byte; /* holds the 32-bit little-endian offset of the byte after the last byte that was written */
	char reserved[44];
};

#define LOG_HEADER_LAST_BYTE_OFFSET 16 /*size of magic word + buf start + buf length*/
#define LOG_HEADER_FLAG_BUFFER_WRAPAROUND 0x80000000
#define LOG_MAJOR_VERSION 1
#define LOG_MINOR_VERSION 0

#endif /* __LOG_H */
