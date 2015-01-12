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

#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "general.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "cmdif_client_aiop.h"
#include "cmdif.h"
#include "fsl_cmdif_fd.h"
#include "fsl_cmdif_flib_c.h"
#include "fsl_dprc.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_endian.h"
#include "fsl_general.h"
#include "ls2085_aiop/fsl_platform.h"
#include "fsl_spinlock.h"
#include "fsl_io_ccsr.h"
#include "fsl_icontext.h"

#define CMDIF_TIMEOUT     0x10000000

void cmdif_client_free();
int cmdif_client_init();
void cmdif_cl_isr();

__TASK static struct ldpaa_fd _fd __attribute__((aligned(sizeof(struct ldpaa_fd))));

extern struct icontext icontext_aiop;

static inline int send_fd(int pr, void *_sdev)
{
	struct cmdif_reg *sdev = (struct cmdif_reg *)_sdev;

#ifdef DEBUG
	if ((sdev == NULL) 				||
		(sdev->attr->num_of_priorities <= pr))
		return -EINVAL;
#endif
	/* Copy fields from FD  */
	_fd.control = 0;
	_fd.offset  = (((uint32_t)FD_IVP_MASK) << 8); /* IVP */

	if (sdev->dma_flags & FDMA_DMA_BMT_BIT)
		_fd.offset |= (((uint32_t)FD_BMT_MASK) << 8);
	/* TODO check about VA, eVA bit */
	if (sdev->dma_flags & FDMA_DMA_eVA_BIT)
		_fd.control |= (((uint32_t)FD_VA_MASK) << 8);
	
	_fd.control = CPU_TO_LE32(_fd.control);
	_fd.offset  = CPU_TO_LE32(_fd.offset);
	
/*
	pr_debug("Sending to fqid 0x%x fdma enq flags = 0x%x icid = 0x%x\n", \
	         sdev->tx_queue_attr[pr]->fqid, sdev->enq_flags, sdev->icid);
*/

	if (fdma_enqueue_fd_fqid(&_fd, 
	                         sdev->enq_flags , 
	                         sdev->tx_queue_attr[pr]->fqid, 
	                         sdev->icid)) {	
		return -EIO;
	}

	return 0;
}

static int session_get(const char *m_name,
                                  uint8_t ins_id,
                                  uint32_t dpci_id,
                                  struct cmdif_desc *cidesc)
{
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int i = 0;

	/* TODO if sync mode is supported
	 * Sharing the same auth_id will require management of opened or not
	 * there won't be 2 cidesc with same auth_id because
	 * the same sync buffer is going to be used for 2 cidesc
	 * but as for today we don't support sync on AIOP client
	 * that's why it is working */
	ASSERT_COND_LIGHT(cl != NULL);
	lock_spinlock(&cl->lock);

	i = cmdif_cl_session_get(cl, m_name, ins_id, dpci_id);
	if (i >= 0) {
		struct cmdif_dev *dev = (struct cmdif_dev *)cl->gpp[i].dev;
		cidesc->regs = (void *)cl->gpp[i].regs;
		cidesc->dev  = (void *)dev;
		unlock_spinlock(&cl->lock);
		/* Must be here to prevent deadlocks because 
		 * the same lock is used */
		return icontext_get((uint16_t)dpci_id, \
		             (struct icontext *)(&(dev->reserved[0])));
	}

	unlock_spinlock(&cl->lock);
	return -ENAVAIL;
}

__COLD_CODE int cmdif_client_init()
{
	int err = 0;
	int i   = 0;
	struct cmdif_cl *cl = NULL;

	if (sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL)) {
		pr_err("Client had been already setup\n");
		return -ENODEV;
	}

	/*cl = fsl_os_xmalloc(sizeof(struct cmdif_cl),
	                    MEM_PART_SH_RAM,
	                    8);
	                    */
	cl = fsl_malloc(sizeof(struct cmdif_cl),
		                    8);
	if (cl == NULL) {
		pr_err("No memory for client handle\n");
		return -ENOMEM;
	}

	memset(cl, 0, sizeof(struct cmdif_cl));

	for (i = 0; i < CMDIF_MN_SESSIONS; i++) {
		/*cl->gpp[i].regs = fsl_os_xmalloc(sizeof(struct cmdif_reg),
		                                 MEM_PART_SH_RAM,
		                                 8);*/
		cl->gpp[i].regs = fsl_malloc(sizeof(struct cmdif_reg),
		                                 8);
		/*cl->gpp[i].dev = fsl_os_xmalloc(sizeof(struct cmdif_dev),
		                                 MEM_PART_SH_RAM,
		                                 8);*/
		cl->gpp[i].dev = fsl_malloc(sizeof(struct cmdif_dev),
				                                 8);
		if ((cl->gpp[i].regs == NULL) || (cl->gpp[i].dev == NULL)) {
			pr_err("No memory for client handle\n");
			return -ENOMEM;
		}
		memset(cl->gpp[i].regs, 0, sizeof(struct cmdif_reg));
		memset(cl->gpp[i].dev, 0, sizeof(struct cmdif_dev));
		memset(cl->gpp[i].m_name, CMDIF_FREE_SESSION, 
		       sizeof(cl->gpp[i].m_name));
	}


	err = sys_add_handle(cl, FSL_OS_MOD_CMDIF_CL, 1, 0);
	if (err != 0) {
		pr_err("Can't add client handle\n");
		return err;
	}
	return 0;
}

__COLD_CODE void cmdif_client_free()
{
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int i = 0;

	if (cl != NULL) {
		for (i = 0; i < CMDIF_MN_SESSIONS; i++) {
			if (cl->gpp[i].regs != NULL)
				fsl_free(cl->gpp[i].regs);
			if (cl->gpp[i].dev != NULL)
				fsl_free(cl->gpp[i].dev);
		}
		fsl_free(cl);
	}

}

int cmdif_open(struct cmdif_desc *cidesc,
		const char *module_name,
		uint8_t ins_id,
		void *data,
		uint32_t size)
{
	int    err = 0;

#ifdef DEBUG
	if ((data != NULL) || (size > 0))
		return -EINVAL; /* Buffers are allocated by GPP */
#endif
	
	err = session_get(module_name, ins_id, (uint32_t)cidesc->regs, cidesc);
	
/*
	if (err != 0) {
		pr_err("Session not found\n");
	}
*/
	return err;
}

int cmdif_close(struct cmdif_desc *cidesc)
{
	cidesc->regs = 0;
	cidesc->dev  = 0;
	
	return 0;
}

int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int pr,
		uint64_t data,
		cmdif_cb_t *async_cb,
		void *async_ctx)
{
	struct cmdif_fd fd;
	struct cmdif_dev *dev = NULL;
	
#ifdef ARENA_LEGACY_CODE
	int      t   = 0;
	union cmdif_data done;
#endif
	
#ifdef DEBUG
	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return -EINVAL;
#endif	

	if (cmdif_is_sync_cmd(cmd_id))
		return -ENOTSUP;
	
	/* 
	 * AIOP client can't set async callback inside cmdif_cmd() flib because 
	 * it is not FDMA */
	dev = (struct cmdif_dev *)cidesc->dev;	
	
	if (cmd_id & CMDIF_ASYNC_CMD) {
		struct cmdif_async async_data;

		CMDIF_CMD_FD_SET(&fd, dev, data, \
		                 (size - sizeof(struct cmdif_async)), cmd_id);

		/* Write async cb using FDMA */
		async_data.async_cb  = (uint64_t)async_cb;
		async_data.async_ctx = (uint64_t)async_ctx;
		ASSERT_COND_LIGHT(sizeof(struct icontext) <= CMDIF_DEV_RESERVED_BYTES);
		icontext_dma_write((struct icontext *)(&(dev->reserved[0])), 
		                   (uint16_t)sizeof(struct cmdif_async), 
		                   &async_data, 
		                   CMDIF_ASYNC_ADDR_GET(fd.u_addr.d_addr, fd.d_size));
#ifdef DEBUG
		async_data.async_cb  = 0;
		async_data.async_ctx = 0;
		icontext_dma_read((struct icontext *)(&(dev->reserved[0])), 
		                   sizeof(struct cmdif_async),
		                   CMDIF_ASYNC_ADDR_GET(fd.u_addr.d_addr, fd.d_size),
		                   &async_data);
		ASSERT_COND_LIGHT(async_data.async_cb == (uint64_t)async_cb);		
#endif
	} else {
		CMDIF_CMD_FD_SET(&fd, dev, data, size, cmd_id);
	}
	
	/* Copy FD outside send_fd() to save stack and cycles */
	_fd.addr   = CPU_TO_LE64(fd.u_addr.d_addr);
	_fd.flc    = CPU_TO_LE64(fd.u_flc.flc);
	_fd.frc    = CPU_TO_LE32(fd.u_frc.frc);
	_fd.length = CPU_TO_LE32(fd.d_size);
	
	if (send_fd(pr, cidesc->regs)) {
		return -EINVAL;
	}

#ifdef ARENA_LEGACY_CODE
	if (cmdif_is_sync_cmd(cmd_id)) {

		uint64_t p_sync = \
			((struct cmdif_dev *)cidesc->dev)->p_sync_done;
		struct cmdif_reg *sdev = (struct cmdif_reg *)cidesc->regs;

		do {
			 /* This is relevant only for sync mode
			 * which is not supported by AIOP client */
			fdma_dma_data(4, sdev->icid, &done,
			              p_sync, sdev->dma_flags);
			t++;
		} while ((done.resp.done == 0) && (t < CMDIF_TIMEOUT));

		pr_debug("PASSED sync cmd 0x%x \n", cmd_id);
		done.resp.done = 0;
		if (t >= CMDIF_TIMEOUT)
			return -ETIMEDOUT;
		else
			return done.resp.err;
	}
#endif
	
	sl_pr_debug("PASSED sent async or no response cmd 0x%x \n", cmd_id);
	return 0;
}

void cmdif_cl_isr(void)
{
	struct cmdif_fd fd;
	struct cmdif_async async_data;
	
	fd.d_size        = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	fd.u_addr.d_addr = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	fd.u_flc.flc     = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	fd.u_frc.frc     = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);

	/* Read async cb using FDMA */
	sl_pr_debug("Got async response for cmd 0x%x\n", \
		         CPU_TO_SRV16(fd.u_flc.cmd.cmid));
	
	ASSERT_COND_LIGHT((fd.d_size > 0) && (fd.u_addr.d_addr != NULL));
	async_data.async_cb  = 0;
	async_data.async_ctx = 0;
	icontext_dma_read((struct icontext *)(&(CMDIF_DEV_GET(&fd)->reserved[0])), 
	                   (uint16_t)sizeof(struct cmdif_async),
	                   CMDIF_ASYNC_ADDR_GET(fd.u_addr.d_addr, fd.d_size),
	                   &async_data);


	ASSERT_COND_LIGHT(async_data.async_cb);
	/* In WS there must be AIOP ICID */
	SET_AIOP_ICID;
	if (((cmdif_cb_t *)async_data.async_cb)((void *)async_data.async_ctx,
		fd.u_flc.cmd.err,
		CPU_TO_SRV16(fd.u_flc.cmd.cmid),
		fd.d_size,
		(void *)PRC_GET_SEGMENT_ADDRESS())) {
		
		sl_pr_debug("Async callback cmd 0x%x returned error \n", \
		         CPU_TO_SRV16(fd.u_flc.cmd.cmid));
	}

	pr_debug("PASSED got async response for cmd 0x%x\n", \
	         CPU_TO_SRV16(fd.u_flc.cmd.cmid));

	CMDIF_STORE_DATA;
	fdma_terminate_task();
}

