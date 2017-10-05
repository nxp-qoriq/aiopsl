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

#ifndef __SPARSER_DRV_H
#define __SPARSER_DRV_H

/******************************************************************************/
#define SOC_PERIPH_OFF_AIOP_CTLU		0x38000
#define CTLU_PARSER_OFFSET			0

#define PARSER_REGS_ADDR	(AIOP_PERIPHERALS_OFF +			\
				 SOC_PERIPH_OFF_AIOP_TILE +		\
				 SOC_PERIPH_OFF_AIOP_CTLU +		\
				 CTLU_PARSER_OFFSET)

/* Parser enable(d) flag */
#define PARSER_ENABLE			0x01
/* Parser idle flag */
#define PARSER_IDLE			0x01
/* Parser IPv6 atomic fragment detection flag */
#define PARSER_ATOMIC_FRAG_DETECTION_ENABLE	0x02
/* Parser stop retry counter */
#define	PARSER_STOP_RETRY		100

/* Note: Parser register space over loads HXS region of the soft parse
 * addressable space. */
struct parser_regs {
	uint32_t par_cfg;	/* Parser Configuration Register */
	uint32_t par_idle;	/* Parser Idle Register */
	uint32_t par_pclim;	/* Parsing Cycle Limit Register */
	uint32_t par_ip_rev_1;	/* Parser IP Block Revision 1 Reg. */
	uint32_t par_ip_rev_2;	/* Parser IP Block Revision 2 Reg. */
	uint32_t reserved[11];	/* Reserved */
};

int sparser_drv_early_init(void);
int sparser_drv_init(void);
int sparser_drv_get_pmem(uint8_t *pmem);
int sparser_drv_check_params(uint16_t pc, uint8_t param_size, uint8_t param_off,
			     uint8_t prpid, uint8_t ingress);

inline int parser_is_atomic_frag_detection_enabled(void)
{
	return (int)((struct parser_regs *)PARSER_REGS_ADDR)->par_cfg &
		PARSER_ATOMIC_FRAG_DETECTION_ENABLE;
}

#endif		/* __SPARSER_DRV_H */
