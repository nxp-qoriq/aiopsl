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

#ifndef __AIOP_COMMON_H
#define __AIOP_COMMON_H

#define AIOP_EP_TABLE_NUM_OF_ENTRIES	1024
#define AIOP_INIT_DATA_FIXED_ADDR	(uint8_t *)0x01010000

#define AIOP_ATU_NUM_OF_WINDOWS         8

#define AIOP_MAX_COMMAND_LINE_ARGS      512

/**************************************************************************//**
 @Description   EPID table
*//***************************************************************************/
enum aiop_epid_table {
       AIOP_EPID_CMDIF_SERVER    = 0,     /**< EPID for command interface server*/
       AIOP_EPID_TIMER_EVENT_IDX = 1,     /**< EPID for T-MAN block */
       AIOP_EPID_CMDIF_CLIENT    = 2,     /**< EPID for command interface client */
       AIOP_EPID_DPNI_START      = 3,     /**< DPNI's first EPID */
       AIOP_EPID_TABLE_SIZE      = 1024   /**< MAX number of EPID's */
};

/* Internal data exchanged between AIOP and MC
 * FIELDS MUST BE ALIGNED AS PACKED */
struct aiop_app_init_info {

	uint64_t dp_ddr_size; /* initialized by AIOP APP at compile time, default provided */
	uint64_t peb_size; /* initialized by AIOP APP at compile time, default provided */
	uint64_t sys_ddr1_size; /* initialized by AIOP APP at compile time, default provided */

	uint32_t ctlu_sys_ddr_num_entries; /* initialized by AIOP APP at compile time, default provided */
	uint32_t ctlu_dp_ddr_num_entries; /* initialized by AIOP APP at compile time, default provided */
	uint32_t ctlu_peb_num_entries; /* initialized by AIOP APP at compile time, default provided */

	uint32_t mflu_sys_ddr_num_entries; /* initialized by AIOP APP at compile time, default provided */
	uint32_t mflu_dp_ddr_num_entries; /* initialized by AIOP APP at compile time, default provided */
	uint32_t mflu_peb_num_entries; /* initialized by AIOP APP at compile time, default provided */

	uint32_t sru_size;
	uint32_t tasks_per_core;

	uint32_t spid_count;

	uint32_t reserved[49]; /* reserved for future use. Keep 256B */
};

/* Internal data exchanged between AIOP and MC
 * FIELDS MUST BE ALIGNED AS PACKED */
struct aiop_sl_init_info
{
	uint32_t aiop_rev_major;  /* initialized by AIOP SL at compile time */
	uint32_t aiop_rev_minor; /* initialized by AIOP SL at compile time */
	uint32_t aiop_revision;  /* initialized by AIOP SL at compile time */

	uint32_t base_spid;

	uint64_t dp_ddr_paddr;
	uint64_t dp_ddr_vaddr;/*virtual base address, initialized by MC FW before AIOP elf is loaded */

	uint64_t peb_paddr;
	uint64_t peb_vaddr; /* virtual base address, initialized by MC FW before AIOP elf is loaded */

	uint64_t sys_ddr1_paddr;
	uint64_t sys_ddr1_vaddr;/*  virtual base address, initialized by MC FW before AIOP elf is loaded */

	uint64_t ccsr_paddr;
	uint64_t ccsr_vaddr;/*  virtual base address, initialized by MC FW before AIOP elf is loaded */

	uint64_t mc_portals_paddr;
	uint64_t mc_portals_vaddr;/*  virtual base address, initialized by MC FW before AIOP elf is loaded */

	uint32_t uart_port_id;      /* initialized by MC FW during init, before AIOP elf is loaded */
	uint32_t mc_portal_id;                  /* initialized by MC FW during init, before AIOP elf is loaded */
	uint32_t mc_dpci_id;                    /* initialized by MC FW during init, before AIOP elf is loaded */
	uint32_t clock_period; 		/* 1588 period In nanosec */

	uint64_t log_buf_paddr; 	/* physical address of log buffer */
	uint32_t log_buf_size;
	uint32_t sys_clk; /* in Khtz */

	uint64_t options;
	uint32_t args_size;	/* AIOP command line string length */
	uint8_t args[AIOP_MAX_COMMAND_LINE_ARGS];	/* AIOP command line string */

	uint32_t reserved[29];           /* reserved for future use. Keep 768B (3/4 K) */
};


struct aiop_init_info
{
	struct aiop_sl_init_info  sl_info;
	struct aiop_app_init_info app_info;
};

/**************************************************************************//**
 @Description   AIOP tile and AIOP blocks registers
 *//***************************************************************************/
struct aiop_cmgw_regs {
	/* General Configuration Port Control and Status Registers */
	uint32_t tcr; /* Tile control register */
	uint32_t tsr; /* Tile status register */
	uint32_t ip_rev_1; /* IP block revision 1 register */
	uint32_t ip_rev_2; /* IP block revision 2 register */
	uint8_t reserved1[0x10];

	/* Tile Global Control and Status Registers */
	uint32_t wscr; /* Workspace control register */
	uint32_t rgcr; /* Region guarded control register */
	uint8_t reserved2[0x8];
	uint32_t tscru; /* 1588 time stamp capture register (upper portion) */
	uint32_t tscrl; /* 1588 time stamp capture register (lower portion) */
	uint32_t tsrr; /* 1588 time stamp resolution register */
	uint8_t reserved3[0x4];
	uint32_t amq_ste_cr; /* AMQ STE control register */
	uint8_t reserved4[0x3C];

	/* AIOP Core Control Registers */
	uint32_t rbase; /* Reset base address register */
	uint8_t reserved5[0x4];
	uint32_t sdabase; /* SDA base address register */
	uint32_t sda2base; /* SDA2 base address register */
	uint32_t abrr; /* AIOP boot release request register */
	uint32_t abrcr; /* AIOP boot release clear register */
	uint32_t abcr; /* AIOP boot completion register */
	uint32_t abccr; /* AIOP boot completion clear register */
	uint8_t reserved6[0x60];

	/* Error Management and Interrupt */
	uint32_t acintrqlr; /* AIOP core interrupt request LP register */
	uint32_t acintclrlr; /* AIOP core interrupt clear LP register  */
	uint32_t acintrqhr; /* AIOP core interrupt request HP register */
	uint32_t acintclrhr; /* AIOP core interrupt clear HP register */
	uint8_t reserved7[0xC];
	uint32_t rstcr; /* Reset Control Register */
	uint8_t reserved7a[0x20];

	/* Management General Purpose Registers */
	uint32_t mgpr[8]; /* Management general purpose registers 0-7 */
	uint8_t reserved8[0x20];

	/* AIOP Core General Purpose Registers */
	uint32_t acgpr[16]; /* AIOP core general purpose registers 0-15 */
	uint8_t reserved9[0x40];

	/* AIOP Block Registers */
	uint32_t stecr1; /* Statistics engine control register 1 */
	uint8_t reserved10[0xFC];

	/* AIOP Discovery Registers */
	uint32_t tile_disc[4]; /* Tile discovery registers 1-4 */
	uint8_t reserved11[0xCF0];
};

struct aiop_fdma_regs {
	/* Global Common Configuration */
	uint32_t cfg; /* Configuration register */
	uint8_t reserved1[0xC];
	uint32_t hcl; /* Hop count limit register */
	uint32_t smcacr; /* System memory cache attribute control register */
	uint8_t reserved2[0xE8];

	/* System Memory Access Configuration */
	uint32_t sru_base_l; /* SRU Base register low */
	uint32_t sru_base_h; /* SRU Base register high */
	uint32_t sru_size; /* SRU Size register */
	uint32_t sru_hwm; /* SRU High Water Mark register */
	uint8_t reserved3[0x2E8];

	uint32_t ip_rev_1; /* IP Block Revision 1 register */
	uint32_t ip_rev_2; /* IP Block Revision 2 register */
	uint8_t reserved4[0xC00];
};

struct aiop_cdma_regs {
	/* Global Common Configuration */
	uint32_t cfg; /* Configuration register */
	uint8_t reserved1[0x10];
	uint32_t smcacr; /* System memory cache attribute control register */
	uint8_t reserved2[0x3E0];

	/* System Memory Access Configuration */
	uint32_t ip_rev_1; /* IP Block Revision 1 register */
	uint32_t ip_rev_2; /* IP Block Revision 2 register */
	uint8_t reserved3[0xC00];
};

struct aiop_osm_regs {
	uint32_t omr; /* OSM mode register */
	uint32_t osr; /* OSM status register */
	uint8_t reserved1[0x18];
	uint32_t ortar; /* OSM read task address register */
	uint8_t reserved2[0x4];
	uint32_t ortdr[8]; /* OSM read task data registers 0-7 */
	uint8_t reserved3[0x58];
	uint32_t oemvr; /* OSM event monitoring value register */
	uint32_t oemmr; /* OSM event monitoring mask register */
	uint8_t reserved4[0xB54];
	uint32_t ocr; /* OSM capability register */
	uint8_t reserved5[0x200];
	uint32_t oerr; /* OSM error report register */
	uint32_t oedr; /* OSM error detect register */
	uint32_t oeddr; /* OSM error detect disable register */
	uint8_t reserved6[0x14];
	uint32_t oecr[8]; /* OSM error capture registers 0-7 */
	uint8_t reserved7[0xC0];
	uint32_t oeuomr; /* OSM engineering use only mode register */
	uint8_t reserved8[0xFC];
};

struct aiop_ws_flow_ctrl_class {
	uint32_t fc_cgmc; /* Flow control congestion group matching class */
	uint32_t fc_bpmc; /* Flow control buffer pool matching class */
	uint32_t fc_fqid; /* Flow control FQID */
	uint8_t reserved[0x4];
};

struct aiop_ws_regs {
	/* Global Common Configuration */
	uint32_t cfg; /* Configuration register */
	uint8_t reserved0[0xC];
	uint32_t idle; /* Activity status register */
	uint32_t cstop; /* Core stop register */
	uint32_t cpend; /* Core pending status register */
	uint32_t cidle; /* Core idle status register */
	uint32_t ecclog; /* 2-bit ECC event log */
	uint8_t reserved2[0x4C];
	uint32_t fc_cgbcf; /* Flow control congestion group broadcast filter */
	uint32_t fc_bpbcf; /* Flow control buffer pool broadcast filter */
	uint8_t reserved3[0x58];
	uint32_t db_cfga; /* Debug configuration A */
	uint32_t db_cfgb; /* Debug configuration B */
	uint8_t reserved4[0x20];

	/* Entry Point ID Look Up Table Configuration */
	uint32_t epas; /* Entry point access select  */
	uint8_t reserved5[0x4];
	uint32_t ep_pc; /* Entry point program counter */
	uint32_t ep_pm; /* Entry point parameter */
	uint32_t ep_fdpa; /* Entry point frame descriptor presentation address */
	uint32_t ep_ptapa; /* Entry point pass through annotation presentation address */
	uint32_t ep_asapa; /* Entry point accelerator specific annotation presentation address */
	uint32_t ep_spa; /* Entry point segment presentation address */
	uint32_t ep_spo; /* Entry point segment presentation offset */
	uint32_t ep_osc; /* Entry point order scope construction */
	uint8_t reserved6[0xD8];

	/* Channel Configuration */
	uint32_t cas; /* Channel access select */
	uint8_t reserved7[0x4];
	uint32_t ch_cfga; /* Channel configuration A */
	uint32_t ch_cfgb; /* Channel configuration B */
	uint8_t reserved8[0xF4];
	uint32_t fc_cfg; /* Flow control configuration */
	struct aiop_ws_flow_ctrl_class class_regs[8];
	/* Class flow control registers */
	uint8_t reserved9[0x60];

	/* Statistics */
	uint32_t spare0;
	uint32_t spare1;
	uint32_t spare2;
	uint32_t spare3;
	uint8_t reserved10[0x8];

	/* Block ID */
	uint32_t ip_rev_1; /* IP Block Revision 1 */
	uint32_t ip_rev_2; /* IP Block Revision 2 */

	uint8_t reserved11[0xC00];
};

struct aiop_atu_win_regs {
	uint32_t acore_owbar; /* AIOP core outbound window base address register */
	uint32_t acore_owar; /* AIOP core outbound window attributes register */
	uint32_t acore_otear; /* AIOP core outbound translated extended address register */
	uint32_t acore_otar; /* AIOP core outbound translated address register */
};

struct aiop_atu_regs {
	/* ATU Control and Status Registers */
	uint32_t atucr; /* ATU Control Register */
	uint32_t atusr; /* ATU Status Register */
	uint32_t ip_rev_1; /* IP Block Revision 1 Register */
	uint32_t ip_rev_2; /* IP Block Revision 2 Register */
	uint8_t reserved1[0xC];
	uint32_t pmcr; /* Performance Monitor Register */
	uint8_t reserved2[0xE0];

	/* ATU Outbound Windows Registers */
	struct aiop_atu_win_regs win_regs[AIOP_ATU_NUM_OF_WINDOWS];
	uint8_t reserved3[0xC80];

	/* ATU Error Registers */
	uint32_t e_capt_address; /* Error Address Capture Register */
	uint32_t e_capt_attrib; /* Error Attributes Capture Register */
	uint8_t reserved4[0x1F8];
};

struct aiop_pm_regs {
	uint32_t pmpgdcr; /* Dynamic Power Gating Control Register */
	uint8_t reserved0[0xC];
	uint32_t tpmhr; /* Dynamic Power Gating Control Register */
	uint32_t tmphaltedr; /* Dynamic Power Gating Control Register */
	uint32_t cpmh; /* Core PM Halt */
	uint32_t cpmhd; /* Core PM Halted */
	uint32_t copms; /* Cluster PM Stop */
	uint32_t copmsd; /* Cluster PM Stopped */
	uint32_t clpmr; /* Cluster PM Retain */
	uint32_t clpmrd; /* Cluster PM Retained */
	uint8_t reserved1[0xFD0];
};

struct aiop_tmi_regs {
	uint32_t tmstatntc; /* TMan Stats Num of Timers Created */
	uint32_t tmstatntd; /* TMan Stats Num of Timers Deleted */
	uint32_t tmstatnat; /* TMan Stats Num of Active Timers */
	uint32_t tmstatnccp; /* TMan Stats Number of Callback Confirmation Pending */
	uint32_t tmstatntf; /* TMan Stats Number of Timers Fired */
	uint32_t tmstatnti; /* TMan Stats Number of Tasks Initiated */
	uint32_t tmstate; /* TMan State */
	uint8_t reserved[0x4];
};

struct aiop_tman_regs {
	uint32_t tmcfg; /* TMan configuration register */
	uint32_t tmamq; /* TMan AMQ */
	uint32_t tmbal; /* TMan external memory base address low */
	uint32_t tmbah; /* TMan external memory base address high */
	uint32_t tminit; /* TMan initialization register */
	uint32_t tmcbcc; /* TMan callback completion confirmation */
	uint8_t reserved1[0x8];
	uint32_t tmtstmpl; /* TMan Timestamp Low */
	uint32_t tmtstmph; /* TMan Timestamp High */
	uint8_t reserved2[0x1FD8];

	struct aiop_tmi_regs tmi_regs[252];
	uint8_t reserved3[0x80];
};

struct aiop_tile_regs {
	struct aiop_cmgw_regs cmgw_regs; /* Configuration management gateway: tile configuration registers */
	uint8_t reserved1[0xB000];
	struct aiop_fdma_regs fdma_regs; /* Frame DMA Registers */
	struct aiop_cdma_regs cdma_regs; /* Context DMA Registers */
	uint8_t reserved2[0xE000];
	struct aiop_osm_regs osm_regs; /* Ordering Scope Manager Registers */
	struct aiop_ws_regs ws_regs; /* Work Scheduler Registers */
	struct aiop_atu_regs atu_regs; /* Address Translation Unit Registers */
	struct aiop_pm_regs pm_regs; /* Power Management Registers */
	struct aiop_tman_regs tman_regs; /* Timer Manager - Configuration Access Port */
	uint8_t reserved3[0x3C000];
};

#endif /* __AIOP_COMMON_H */
