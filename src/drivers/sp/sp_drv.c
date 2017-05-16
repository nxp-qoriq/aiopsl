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
#include "fsl_types.h"
#include "fsl_sl_slab.h"
#include "fsl_dprc.h"
#include "fsl_platform.h"
#include "fsl_dpbp.h"
#include "fsl_bman.h"
#include "fsl_icontext.h"
#include "fsl_mem_mng.h"
#include "common/fsl_string.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_sp_drv.h"
#include "sp_drv.h"

/******************************************************************************/
#define MAX_SP			256
#define MAX_SP_BP		2

#define	INVALID_BP		0xFFFF
#define	INVALID_SP		0xFF

#define SP_DEFAULT_DHR		256
#define SP_DHR_MASK		0x00000FFF
#define SP_FF_MASK		0x30000000
#define SP_VA			0x04000000
#define SP_PTAR			0x00800000
#define SP_SGHR			0x00100000
#define SP_BP_SR		0x00000020
#define SP_BP_BP		0x00000002
#define SP_ASAR_MASK		0x000F0000
#define AIOP_AMQ_PL		0x00040000
#define AIOP_SP_BDI		0x00080000
#define AIOP_AMQ_ICID_MASK	0x00007FFF

/* Buffer Pool Entry Configuration Structure */
struct sp_bp_entry_info {
	enum memory_partition_id	mem_pid;
	uint32_t			buffer_cnt;
	uint16_t			buffer_size;
	uint16_t			alignment;
	uint16_t			bpid;
	int				bp_obj_id;
};

/* Storage Profile Entry Configuration Structure */
struct sp_entry_info {
	uint8_t				spid;
	uint8_t				bp_count;
	struct sp_bp_entry_info		bpe[MAX_SP_BP];
};

/* Storage Profile Configuration Structure */
struct sp_info {
	uint8_t				sp_count;
	uint8_t				bp_count;
	uint8_t				base_spid;
	struct sp_entry_info		*spe;
};

struct sp_info		sp_info;

/* Storage Profile Entry Structure */
struct aiop_psram_entry {
	uint32_t	aiop_specific;
	uint32_t	reserved0;
	uint32_t	frame_format_low;
	uint32_t	frame_format_high;
	uint32_t	bp1;
	uint32_t	bp2;
	uint32_t	reserved1;
	uint32_t	reserved2;
};

#ifdef SL_DEBUG
struct be_storage_profile {
	uint64_t ip_secific_sp_info; /* IP-Specific SP Information */

	uint16_t reserved;	/* reserved */
	uint16_t dl;		/* DataLength(correction) */
	uint8_t  mode_bits2;	/* mode bits */
	uint8_t  mode_bits1;	/* mode bits */
	uint16_t dhr;		/* DataHeadRoom(correction) */
	uint16_t bpid1;		/* Bypass Memory Translation */
	uint16_t pbs1;		/* Pool Buffer Size */
	uint16_t bpid2;		/* Bypass Memory Translation */
	uint16_t pbs2;		/* Pool Buffer Size */
	uint32_t reserved1;
	uint32_t reserved2;
};
#endif

/*******************************************************************************
 * Local functions
 */
/******************************************************************************/
#ifdef SL_DEBUG
static __COLD_CODE void storage_profile_to_cpu(uint32_t *src,
					       struct aiop_psram_entry *sp)
{
	sp->aiop_specific = LW_SWAP(0, src++);
	sp->reserved0 = LW_SWAP(0, src++);
	sp->frame_format_low = LW_SWAP(0, src++);
	sp->frame_format_high = LW_SWAP(0, src++);
	sp->bp1 = LW_SWAP(0, src++);
	sp->bp2 = LW_SWAP(0, src++);
	sp->reserved1 = LW_SWAP(0, src++);
	sp->reserved2 = LW_SWAP(0, src);
}

/******************************************************************************/
static __COLD_CODE void dump_bpe(struct be_storage_profile *sp, uint8_t bp_idx)
{
	uint16_t	pbs, bpid;

	if (!bp_idx) {
		pbs = sp->pbs1;
		bpid = sp->bpid1;
	} else {
		pbs = sp->pbs2;
		bpid = sp->bpid2;
	}
	fsl_print("\t Buffer Pool %d Controls\n", bp_idx + 1);
	fsl_print("\t\t BMT  = %d\n", bpid & 0x8000 ? 1 : 0);
	fsl_print("\t\t BPID = %d\n", bpid & 0x3fff);
	fsl_print("\t\t PBS  = %d\n", ((pbs >> 6) & 0x3ff) * 64);
	fsl_print("\t\t SR  = %d\n", pbs & 0x20 ? 1 : 0);
	fsl_print("\t\t BDP = %d\n", pbs & 0x4 ? 1 : 0);
	fsl_print("\t\t BP  = %d\n", pbs & 0x2 ? 1 : 0);
	fsl_print("\t\t BPV = %d\n", pbs & 0x1 ? 1 : 0);
}
#endif

/******************************************************************************/
static __COLD_CODE int get_sp_dpbp(int **dev_idx, int count,
				   struct sp_entry_info *spe,
				   struct fsl_mc_io *mc_io, uint16_t token)
{
	uint8_t				bp_idx;
	struct dprc_obj_desc		dev_desc;

	bp_idx = 0;
	for ( ; *(*dev_idx) < count &&
	     bp_idx < spe->bp_count; *(*dev_idx) += 1) {
		dprc_get_obj(mc_io, 0, token, *(*dev_idx), &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			pr_info("SPID %d : dpbp@%d\n", spe->spid,
				dev_desc.id);
			spe->bpe[bp_idx++].bp_obj_id = dev_desc.id;
		}
	}
	if (bp_idx < spe->bp_count) {
		pr_err("Failed to reserve %d BP resources for SPID = %d\n",
		       spe->bp_count, spe->spid);
		return -ENOENT;
	}
	return 0;
}

/******************************************************************************/
static __COLD_CODE void sp_set_amq_and_dhr(uint8_t spid)
{
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;

	spe = (struct aiop_psram_entry *)&storage_profile[spid];
	/* ICID */
	sp_word = (uint32_t)(icontext_aiop.icid & AIOP_AMQ_ICID_MASK);
	/* PL */
	sp_word |= AIOP_AMQ_PL;
	/* BDI */
	sp_word |= AIOP_SP_BDI;
	STW_SWAP(sp_word, 0,  &spe->aiop_specific);
	/* Setting DHR (data head room) */
	sp_word = SP_DEFAULT_DHR & SP_DHR_MASK;
	STW_SWAP(sp_word, 0,  &spe->frame_format_high);
}

/******************************************************************************/
static __COLD_CODE void sp_set_bpid(struct sp_entry_info *cfg_spe,
				    uint8_t bp_idx)
{
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;

	/* Setting Buffer Pool Attributes and Controls */
	/* BMT and BVP */
	sp_word = (icontext_aiop.bdi_flags & FDMA_ENF_BDI_BIT) | 1;
	/* BPID */
	sp_word |= cfg_spe->bpe[bp_idx].bpid << 16;
	/* PBS */
	sp_word |= (cfg_spe->bpe[bp_idx].buffer_size / 64) << 6;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	switch (bp_idx) {
	case 0:
		STW_SWAP(sp_word, 0, &spe->bp1);
		break;
	case 1:
	default:
		STW_SWAP(sp_word, 0, &spe->bp2);
		break;
	}
}

/******************************************************************************/
static __COLD_CODE int sp_fill_dpbp(struct sp_entry_info *spe,
				    struct fsl_mc_io *mc_io)
{
	struct dpbp_attr		attr;
	uint8_t				bp_idx;
	uint16_t			dpbp;
	int				dpbp_id, err;

	dpbp = 0;	/* Klockwork */
	for (bp_idx = 0; bp_idx < spe->bp_count; bp_idx++) {
		/* Not configured BP */
		if (spe->bpe[bp_idx].bp_obj_id == -1)
			continue;
		dpbp_id = spe->bpe[bp_idx].bp_obj_id;
		/* Open DPBP control session */
		err = dpbp_open(mc_io, 0, dpbp_id, &dpbp);
		if (err) {
			pr_err("Open dpbp@%d\n", dpbp_id);
			dpbp_close(mc_io, 0, dpbp);
			break;
		}
		err = dpbp_enable(mc_io, 0, dpbp);
		if (err) {
			pr_err("Enable dpbp@%d\n", dpbp_id);
			dpbp_close(mc_io, 0, dpbp);
			break;
		}
		err = dpbp_get_attributes(mc_io, 0, dpbp, &attr);
		if (err) {
			pr_err("Get attributes of dpbp@%d\n", dpbp_id);
			dpbp_close(mc_io, 0, dpbp);
			break;
		}
		pr_info("Fill dpbp@%d : cnt = %d sz = %d al = %d mem = %s\n",
			dpbp_id, spe->bpe[bp_idx].buffer_cnt,
			spe->bpe[bp_idx].buffer_size,
			spe->bpe[bp_idx].alignment,
			(spe->bpe[bp_idx].mem_pid == MEM_PART_PEB) ? "PEB" :
			(spe->bpe[bp_idx].mem_pid == MEM_PART_DP_DDR) ?
			"DP-DDR" : "SYS_DDR");
		err = bman_fill_bpid(spe->bpe[bp_idx].buffer_cnt,
				     spe->bpe[bp_idx].buffer_size,
				     spe->bpe[bp_idx].alignment,
				     spe->bpe[bp_idx].mem_pid, attr.bpid, 0);
		if (err) {
			pr_err("Fill dpbp@%d\n", dpbp_id);
			dpbp_close(mc_io, 0, dpbp);
			break;
		}
		spe->bpe[bp_idx].bpid = attr.bpid;
		pr_info("Filled dpbp@%d : BPID = %d\n", dpbp_id, attr.bpid);
		/* Close DPBP control session */
		err = dpbp_close(mc_io, 0, dpbp);
		if (err) {
			pr_err("Close dpbp@%d\n", dpbp_id);
			break;
		}
		/* Set SP parameters in SP storage memory */
		if (!bp_idx)
			sp_set_amq_and_dhr(spe->spid);
		sp_set_bpid(spe, bp_idx);
	}
	if (bp_idx < spe->bp_count) {
		pr_err("Failed to fill %d BP resources for SPID = %d\n",
		       spe->bp_count, spe->spid);
		return err;
	}
	return 0;
}

/******************************************************************************/
static __COLD_CODE void sp_emty_all_dpbp(void)
{
	uint8_t				sp_idx, bp_idx;
	int				err;
	uint32_t			buffer_cnt;
	uint64_t			dst;
	struct sp_entry_info		*spe;

	spe = sp_info.spe;
	for (sp_idx = 0; sp_idx < sp_info.sp_count; sp_idx++, spe++) {
		/* No BP configured on this SP */
		if (spe->spid == INVALID_SP)
			continue;
		buffer_cnt = 0;
		pr_info("SPID = %d : Free %d buffer pools\n",
			spe->spid, spe->bp_count);
		for (bp_idx = 0; bp_idx < spe->bp_count; bp_idx++) {
			/* Not configured BP */
			if (spe->bpe[bp_idx].bpid == INVALID_BP)
				continue;
			pr_info("BPID = %d : Should acquire %d buffers\n",
				spe->bpe[bp_idx].bpid,
				spe->bpe[bp_idx].buffer_cnt);
			do {
				err = fdma_acquire_buffer
				(icontext_aiop.icid,
				 icontext_aiop.bdi_flags & FDMA_ENF_BDI_BIT,
				 spe->bpe[bp_idx].bpid, &dst);
				if (!err)
					buffer_cnt++;
			} while (!err);
			if (buffer_cnt != spe->bpe[bp_idx].buffer_cnt) {
				pr_err("Not all buffers were acquired\n");
				pr_err("Populated : %d\n",
				       spe->bpe[bp_idx].buffer_cnt);
				pr_err("Acquired  : %d\n", buffer_cnt);
			}
			spe->bpe[bp_idx].bpid = INVALID_BP;
		}
		spe->bp_count = 0;
		spe->spid = INVALID_SP;
	}
	sp_info.sp_count = 0;
}

/******************************************************************************/
static __COLD_CODE void sp_clear_all(void)
{
	uint8_t				sp_idx;
	struct sp_entry_info		*cfg_spe;
	struct aiop_psram_entry		*spe;

	cfg_spe = sp_info.spe;
	for (sp_idx = 0; sp_idx < sp_info.sp_count; sp_idx++, cfg_spe++) {
		/* No BP configured on this SP */
		cfg_spe->bp_count = 0;
		cfg_spe->spid = INVALID_SP;
		spe = (struct aiop_psram_entry *)
				&storage_profile[sp_info.base_spid + sp_idx];
		memset(spe, 0, sizeof(struct aiop_psram_entry));
	}
	sp_info.sp_count = 0;
}

/******************************************************************************/
static __COLD_CODE int sp_check_sp_index(uint8_t sp_idx)
{
	struct sp_entry_info		*cfg_spe;

	if (!sp_info.sp_count) {
		pr_err("%d : No SPs reserved in application\n",
		       sp_info.sp_count);
		return -EINVAL;
	}
	if (sp_idx >= sp_info.sp_count) {
		pr_err("%d : Invalid SP index. Must be less than %d\n", sp_idx,
		       sp_info.sp_count);
		return -EINVAL;
	}
	cfg_spe = sp_info.spe + sp_idx;
	if (!cfg_spe->bp_count) {
		pr_err("%d : No BP requested on SP at index %d\n",
		       cfg_spe->bp_count, sp_idx);
		return -EIO;
	}
	return 0;
}

/*******************************************************************************
 * AIOP_SL internal functions
 */
/******************************************************************************/
__COLD_CODE int sp_drv_early_init(void)
{
	int				i, j;
	struct sp_entry_info		*cfg_spe;
	struct aiop_psram_entry		*spe;

	sp_info.sp_count = (uint8_t)g_init_data.app_info.spid_count;
	if (!sp_info.sp_count) {
		pr_warn("No Storage Profiles were reserved\n");
		return 0;
	}
	sp_info.spe = (struct sp_entry_info *)
		fsl_malloc(sp_info.sp_count *
			   sizeof(struct sp_entry_info), 1);
	if (!sp_info.spe) {
		pr_err("Can't allocate memory\n");
		return -ENOMEM;
	}
	pr_info("%d Storage Profiles were reserved by the application\n",
		sp_info.sp_count);
	sp_info.base_spid = (uint8_t)g_init_data.sl_info.base_spid;
	sp_info.bp_count = 0;
	pr_info("Starting SPID : %d\n", sp_info.base_spid);
	memset(sp_info.spe, 0,
	       sp_info.sp_count * sizeof(struct sp_entry_info));
	cfg_spe = sp_info.spe;
	for (i = 0; i < sp_info.sp_count; i++, cfg_spe++) {
		cfg_spe->spid = INVALID_SP;
		for (j = 0; j < MAX_SP_BP; j++) {
			cfg_spe->bpe[j].bpid = INVALID_BP;
			cfg_spe->bpe[j].bp_obj_id = -1;
		}
		spe = (struct aiop_psram_entry *)
				&storage_profile[sp_info.base_spid + i];
		memset(spe, 0, sizeof(struct aiop_psram_entry));
	}
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_init(void)
{
	struct mc_dprc			*dprc;
	struct dprc_obj_desc		dev_desc;
	int				i, *pidx, err, count;
	uint8_t				sp_idx, num_bpids,
					bp_skip_count;
	struct sp_entry_info		*spe;

	if (!sp_info.sp_count) {
		pr_warn("No Storage Profiles were reserved\n");
		return 0;
	}
	pr_info("%d Storage Profiles are reserved by application\n",
		sp_info.sp_count);
	if (!sp_info.bp_count) {
		pr_info("No buffer pool requested on the reserved SPs\n");
		return 0;
	}
	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	err = dprc_get_obj_count(&dprc->io, 0, dprc->token, &count);
	if (err) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}
	/* Skip the buffer pool(s) used for DPNIs. One or two BPs are used by
	 * all DPNIs. All DPNIs use a BP with PEB buffers */
	bp_skip_count = 1;
	/* Check if DPNI backup pool is enabled */
	if ((!g_app_params.app_config_flags & DPNI_BACKUP_POOL_DISABLE))
		bp_skip_count++;
	/* Check if IPSec buffer allocate mode is enabled */
	if (g_app_params.app_config_flags & IPSEC_BUFFER_ALLOCATE_ENABLE)
		bp_skip_count++;
	/* Check if CWAP/DTLS buffer allocate mode is enabled */
	if (g_app_params.app_config_flags & CWAP_DTLS_BUFFER_ALLOCATE_ENABLE)
		bp_skip_count++;
	/* Skip the already reserved buffer pools (DPNI, IPSec, CWAP) */
	num_bpids = 0;
	for (i = 0; i < count && num_bpids < bp_skip_count; i++) {
		dprc_get_obj(&dprc->io, 0, dprc->token, i, &dev_desc);
		if (!strcmp(dev_desc.type, "dpbp")) {
			num_bpids++;
			pr_info("Skipped reserved dpbp@%d\n", dev_desc.id);
		}
	}
	if (num_bpids < bp_skip_count) {
		pr_err("Failed to skip %d reserved BPs\n", bp_skip_count);
		return -ENOENT;
	}
	/* Reserve requested BPs for all configured SPs (sp_info.bp_count) */
	spe = sp_info.spe;
	pidx = &i;
	for (sp_idx = 0; sp_idx < sp_info.sp_count; sp_idx++, spe++) {
		/* No BP configured on this SP */
		if (spe->spid == INVALID_SP)
			continue;
		err = get_sp_dpbp(&pidx, count, spe, &dprc->io, dprc->token);
		if (err)
			return err;
	}
	/* Fill configured buffer pools on each SP */
	spe = sp_info.spe;
	for (sp_idx = 0; sp_idx < sp_info.sp_count; sp_idx++, spe++) {
		/* No BP configured on this SP */
		if (spe->spid == INVALID_SP)
			continue;
		err = sp_fill_dpbp(spe, &dprc->io);
		if (err) {
			sp_emty_all_dpbp();
			sp_clear_all();
			return err;
		}
	}
	return 0;
}

/******************************************************************************/
__COLD_CODE void sp_drv_free(void)
{
	if (sp_info.spe) {
		sp_emty_all_dpbp();
		sp_clear_all();
		fsl_free(sp_info.spe);
		sp_info.spe = 0;
	}
}

/******************************************************************************/
__COLD_CODE uint8_t sp_drv_get_bp_count(void)
{
	return sp_info.bp_count;
}

/*******************************************************************************
 * API functions
 */
/******************************************************************************/
__COLD_CODE
int sp_drv_register_bp_requirements(uint8_t sp_idx, uint32_t buffer_cnt,
				    uint16_t buffer_size, uint16_t alignment,
				    enum memory_partition_id mem_pid)
{
	struct sp_entry_info	*spe;

	if (!sp_info.sp_count) {
		pr_warn("%d : No SPs reserved in application\n",
			sp_info.sp_count);
		return 0;
	}
	if (!buffer_cnt) {
		pr_err("%d : No buffers requested\n", buffer_cnt);
		return -EINVAL;
	}
	if (!buffer_size) {
		pr_err("Requested buffers with length of 0 bytes\n",
		       buffer_size);
		return -EINVAL;
	}
	if (sp_idx >= sp_info.sp_count) {
		pr_err("%d : Invalid SP index. Must be less than %d\n", sp_idx,
		       sp_info.sp_count);
		return -EINVAL;
	}
#ifdef LS1088A_REV1
	if (mem_pid != MEM_PART_SYSTEM_DDR && mem_pid != MEM_PART_PEB) {
		pr_err("0x%x : Invalid memory partition\n", (uint32_t)mem_pid);
		return -EINVAL;
	}
#else
	if (mem_pid != MEM_PART_DP_DDR && mem_pid != MEM_PART_SYSTEM_DDR &&
	    mem_pid != MEM_PART_PEB) {
		pr_err("0x%x : Invalid memory partition\n", (uint32_t)mem_pid);
		return -EINVAL;
	}
#endif
	spe = sp_info.spe + sp_idx;
	if (spe->bp_count == MAX_SP_BP) {
		pr_err("Requested more than %d BPs on SP index %d\n",
		       MAX_SP_BP, sp_idx);
		return -EINVAL;
	}
	if (spe->spid == INVALID_SP)
		spe->spid = (uint8_t)(sp_info.base_spid + sp_idx);
	spe->bpe[spe->bp_count].buffer_cnt = buffer_cnt;
	spe->bpe[spe->bp_count].buffer_size = buffer_size;
	spe->bpe[spe->bp_count].alignment = alignment;
	/* Per SP number of requested BPs */
	spe->bpe[spe->bp_count].mem_pid = mem_pid;
	/* Total number of requested BPs */
	spe->bp_count++;
	sp_info.bp_count++;
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_get_spid(uint8_t sp_idx, uint8_t *spid)
{
	struct sp_entry_info	*spe;
	int			err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	spe = sp_info.spe + sp_idx;
	*spid = spe->spid;
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_get_bpid(uint8_t sp_idx, uint8_t bp_idx, uint16_t *bpid)
{
	struct sp_entry_info	*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	spe = sp_info.spe + sp_idx;
	if (bp_idx >= spe->bp_count) {
		pr_err("%d : Invalid BP index. Must be less than %d\n", bp_idx,
		       spe->bp_count);
		return -EINVAL;
	}
	if (spe->bpe[bp_idx].bpid == INVALID_BP) {
		pr_warn("BP %d in SP %d was not yet initialized\n", bp_idx,
			sp_idx);
		return -EIO;
	}
	*bpid = spe->bpe[bp_idx].bpid;
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_aiop_icid(uint8_t sp_idx, uint16_t icid)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->aiop_specific);
	sp_word &= ~AIOP_AMQ_ICID_MASK;
	sp_word |= icid & AIOP_AMQ_ICID_MASK;
	STW_SWAP(sp_word, 0,  &spe->aiop_specific);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_aiop_pl(uint8_t sp_idx, uint8_t pl)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->aiop_specific);
	sp_word &= ~AIOP_AMQ_PL;
	if (pl)
		sp_word |= AIOP_AMQ_PL;
	STW_SWAP(sp_word, 0,  &spe->aiop_specific);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_aiop_bdi(uint8_t sp_idx, uint8_t bdi)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->aiop_specific);
	sp_word &= ~AIOP_SP_BDI;
	if (bdi)
		sp_word |= AIOP_SP_BDI;
	STW_SWAP(sp_word, 0,  &spe->aiop_specific);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_dhr(uint8_t sp_idx, uint16_t dhr)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->frame_format_high);
	sp_word &= ~SP_DHR_MASK;
	sp_word |= (dhr & SP_DHR_MASK);
	STW_SWAP(sp_word, 0,  &spe->frame_format_high);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_ff(uint8_t sp_idx, enum sp_frame_format ff)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->frame_format_high);
	sp_word &= ~SP_FF_MASK;
	sp_word |= (uint32_t)ff;
	STW_SWAP(sp_word, 0,  &spe->frame_format_high);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_va(uint8_t sp_idx, uint8_t va)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->frame_format_high);
	sp_word &= ~SP_VA;
	if (va)
		sp_word |= SP_VA;
	STW_SWAP(sp_word, 0,  &spe->frame_format_high);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_ptar(uint8_t sp_idx, uint8_t ptar)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->frame_format_high);
	sp_word &= ~SP_PTAR;
	if (ptar)
		sp_word |= SP_PTAR;
	STW_SWAP(sp_word, 0,  &spe->frame_format_high);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_sghr(uint8_t sp_idx, uint8_t sghr)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->frame_format_high);
	sp_word &= ~SP_SGHR;
	if (sghr)
		sp_word |= SP_SGHR;
	STW_SWAP(sp_word, 0,  &spe->frame_format_high);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_asar(uint8_t sp_idx, uint8_t asar)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = LW_SWAP(0, &spe->frame_format_high);
	sp_word &= ~SP_ASAR_MASK;
	sp_word |= (asar << 16) & SP_ASAR_MASK;
	STW_SWAP(sp_word, 0,  &spe->frame_format_high);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_bp_sr(uint8_t sp_idx, uint8_t bp_idx, uint8_t sr)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	if (bp_idx >= cfg_spe->bp_count) {
		pr_err("%d : Invalid BP index. Must be less than %d\n", bp_idx,
		       cfg_spe->bp_count);
		return -EINVAL;
	}
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = (bp_idx == 0) ? LW_SWAP(0, &spe->bp1) : LW_SWAP(0, &spe->bp2);
	sp_word &= ~SP_BP_SR;
	if (sr)
		sp_word |= SP_BP_SR;
	STW_SWAP(sp_word, 0,  (bp_idx == 0) ?  &spe->bp1 : &spe->bp2);
	return 0;
}

/******************************************************************************/
__COLD_CODE int sp_drv_set_bp_bp(uint8_t sp_idx, uint8_t bp_idx, uint8_t bp)
{
	struct sp_entry_info		*cfg_spe;
	uint32_t			sp_word;
	struct aiop_psram_entry		*spe;
	int				err;

	err = sp_check_sp_index(sp_idx);
	if (err)
		return err;
	cfg_spe = sp_info.spe + sp_idx;
	if (bp_idx >= cfg_spe->bp_count) {
		pr_err("%d : Invalid BP index. Must be less than %d\n", bp_idx,
		       cfg_spe->bp_count);
		return -EINVAL;
	}
	spe = (struct aiop_psram_entry *)&storage_profile[cfg_spe->spid];
	sp_word = (bp_idx == 0) ? LW_SWAP(0, &spe->bp1) : LW_SWAP(0, &spe->bp2);
	sp_word &= ~SP_BP_BP;
	if (bp)
		sp_word |= SP_BP_BP;
	STW_SWAP(sp_word, 0,  (bp_idx == 0) ?  &spe->bp1 : &spe->bp2);
	return 0;
}

/******************************************************************************/
#ifdef SL_DEBUG
__COLD_CODE void sp_dump(uint8_t sp_idx)
{
	struct aiop_psram_entry		aiop_psram;
	struct be_storage_profile	*sp;
	uint32_t			*src, aiop_specific;
	struct sp_entry_info		*cfg_spe;
	uint16_t			spid;
	uint8_t				val8;

	if (!sp_info.sp_count) {
		pr_err("%d : No SPs reserved in application\n",
		       sp_info.sp_count);
		return;
	}
	if (sp_idx >= sp_info.sp_count) {
		pr_err("%d : Invalid SP index. Must be less than %d\n", sp_idx,
		       sp_info.sp_count);
		return;
	}
	cfg_spe = sp_info.spe + sp_idx;
	spid = cfg_spe->spid;
	src = (uint32_t *)&storage_profile[spid];
	/* Copy current SP and converts it to BE */
	storage_profile_to_cpu(src, &aiop_psram);
	sp = (struct be_storage_profile *)&aiop_psram;
	fsl_print("\nDump of Storage Profile : SPID = %d\n", spid);
	fsl_print("\t AIOP context AMQ parameters\n");
	aiop_specific = (uint32_t)(sp->ip_secific_sp_info >> 32);
	fsl_print("\t\t BDI   = %d\n", aiop_specific & AIOP_SP_BDI ? 1 : 0);
	fsl_print("\t\t PL    = %d\n", aiop_specific & AIOP_AMQ_PL ? 1 : 0);
	fsl_print("\t\t ICID  = 0x%04x\n",
		  (uint16_t)(aiop_specific & AIOP_AMQ_ICID_MASK));
	fsl_print("\t Frame Format and Data Placement Controls\n");
	fsl_print("\t\t DL    = %d\n", sp->dl);
	fsl_print("\t\t BS    = %d\n", (sp->mode_bits2 >> 7) & 0x01);
	val8 = (uint8_t)((sp->mode_bits2 >> 4) & 0x03);
	fsl_print("\t\t FF    = %d (%s)\n", val8,
		  (val8 == 0) ? "Single or SG Buffer" :
		  (val8 == 2) ? "Single Buffer" : "Unsupported");
	fsl_print("\t\t VA    = %d\n", (sp->mode_bits2 >> 2) & 0x01);
	fsl_print("\t\t DLC   = %d\n", sp->mode_bits2 & 0x01);
	val8 = (uint8_t)((sp->mode_bits1 >> 7) & 0x01);
	fsl_print("\t\t PTAR  = %d (%d)\n", val8, 64 * val8);
	val8 = (uint8_t)((sp->mode_bits1 >> 4) & 0x01);
	fsl_print("\t\t SGHR  = %d (%d)\n", val8, 64 * val8);
	val8 = (uint8_t)(sp->mode_bits1 & 0x0f);
	fsl_print("\t\t ASAR  = %d (%d)\n", val8, 64 * val8);
	fsl_print("\t\t DHR   = %d\n", sp->dhr & 0xfff);
	/* BP #1 */
	dump_bpe(sp, 0);
	/* BP #2 */
	dump_bpe(sp, 1);
}
#endif	/* SL_DEBUG */
