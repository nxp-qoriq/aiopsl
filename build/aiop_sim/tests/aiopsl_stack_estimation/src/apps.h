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

/* This value is defined in config.c and holds the size of application in ddr memory*/
extern const uint32_t g_aiop_lcf_ddr_size;

/* This is an application required DP_DDR memory, user SHOULD edit this.
 * In this example the total dp_ddr memory is 128 MB (0x8000000),
 * g_aiop_lcf_ddr_size out of it is occupied by aiop image dp_ddr and the rest is dedicated 
 * for application */
#define APPLICATION_DP_DDR_SIZE (0x8000000 - g_aiop_lcf_ddr_size)
 
/* dp_ddr_size.
 * It is the  sum of AIOP image size in DDR and 
 * application required DP_DDR memory.
 * The value should be aligned to a power of 2 */
#define  AIOP_SL_AND_APP_DDR_SIZE     (uint64_t)(g_aiop_lcf_ddr_size + APPLICATION_DP_DDR_SIZE)	
 
/* peb_size.
 * Should be a power of 2.
 * Applications cannot require more that this maximum size */
#define PEB_SIZE  (512 * KILOBYTE)  

/* sys-ddr1 size = 0. Currently no dynamic allocation from system ddr */
#define SYS_DDR1_SIZE 0

/* ctlu sys-ddr number of entries */
#define CTLU_SYS_DDR_NUM_ENTRIES         2048

/* ctlu dp-ddr number of entries */
#define CTLU_DP_DDR_NUM_ENTRIES          2048

/* ctlu peb number of entries */
#define CTLU_PEB_NUM_ENTRIES             2048

/* mflu sys-ddr number of entries */
#define MFLU_SYS_DDR_NUM_ENTRIES         2048

/* mflu dp-ddr number of entries */
#define MFLU_DP_DDR_NUM_ENTRIES          2048

/* mflu peb number of entries */
#define MFLU_PEB_NUM_ENTRIES             2048

/* sru_size */
#define SRU_SIZE                         0x100000	

/* Tman frequency */
#define TMAN_FREQUENCY                   800

/* Tasks per core in AIOP */
#define AIOP_TASKS_PER_CORE              4

/* Number of frame buffers in pool */
#define DPNI_NUMBER_BUFFERS_IN_POOL      50
/* Size of buffers in pool */
#define DPNI_BUFFER_SIZE_IN_POOL         2048
/* alignment of buffers in pool */
#define DPNI_BUFFER_ALIGNMENT            64
 
/*  Maximal number of applications to be filled by build_apps_array() */
#define APP_MAX_NUM                      10
