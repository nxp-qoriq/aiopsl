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

#include <cmdif.h>
#include <bd_ring/fsl_cmdif_bd_flib.h>

#define CMDIF_RING_SIZE(RING)		(0x1 << (RING)->num_bds)	
/*!< Number of BDs, must be power of 2 */

#define CMDIF_RING_IS_FULL(ENQ, DEQ, SIZE)	\
	(((ENQ) - (DEQ)) == (SIZE))

#define CMDIF_RING_IS_EMPTY(ENQ, DEQ)	\
	(((ENQ) - (DEQ)) == 0)

#define CMDIF_RING_IND(NUM, SIZE) \
	(MODULU_POWER_OF_TWO(NUM, (SIZE))) 
/*!< Always modulu power of 2 */

#define CMDIF_BD_SIZE (sizeof(struct cmdif_bd))

#define CMDIF_BD_WRITE(BASE_ADDR, IND, BD) \
do { \
	*bd_ptr = (struct cmdif_bd *)(BASE_ADDR + CMDIF_BD_SIZE * (IND));\
	**bd_ptr = *(BD); \
}while(0)

#define CMDIF_BD_READ(BASE_ADDR, IND, BD) \
	*(BD) = *((struct cmdif_bd *)(BASE_ADDR + CMDIF_BD_SIZE * (IND)))

#define CMDIF_M_NAME_CHARS	16	/*!< Including null terminator */

int cmdif_flib_send(struct cmdif_bd_ring *bd_ring, const struct cmdif_bd *bd, struct cmdif_bd **bd_ptr) 
{
	uint32_t enq;
	uint32_t deq;
	uint32_t ind;
	uint32_t size;

	if ((bd_ring == NULL) || (bd == NULL))
		return -EINVAL;

	enq = CPU_TO_LE32(bd_ring->enq);
	deq = CPU_TO_LE32(bd_ring->deq);
	size = CMDIF_RING_SIZE(bd_ring);
	if (CMDIF_RING_IS_FULL(enq, deq, size))
		return -ENOMEM;

	ind = CMDIF_RING_IND(enq, size);
	CMDIF_BD_WRITE(bd_ring->bd_addr, ind, bd);

	enq++;
	bd_ring->enq = CPU_TO_LE32(enq);
	return 0;
}

int cmdif_flib_receive(struct cmdif_bd_ring *bd_ring, struct cmdif_bd *bd) 
{
	uint32_t enq;
	uint32_t deq;
	uint32_t ind;
	uint32_t size;

	if ((bd_ring == NULL) || (bd == NULL))
		return -EINVAL;

	enq = CPU_TO_LE32(bd_ring->enq);
	deq = CPU_TO_LE32(bd_ring->deq);
	if (CMDIF_RING_IS_EMPTY(enq, deq))
		return -ENOMEM;

	size = CMDIF_RING_SIZE(bd_ring);
	ind = CMDIF_RING_IND(deq, size);
	CMDIF_BD_READ(bd_ring->bd_addr, ind, bd);

	deq++;
	bd_ring->deq = CPU_TO_LE32(deq);
	return 0;
}

int cmdif_flib_open_bd(void *handle, const char *m_name,
                       struct cmdif_bd *bd)
{
	int i = 0;

	memset(bd, 0, sizeof(*bd));

	/* 15 characters module name terminated by \0*/
	while ((m_name[i] != '\0') && (i < CMDIF_M_NAME_CHARS)) {
		bd->cmd_data[i] = m_name[i];
		i++;
	}
	
	/* cmd_data: module, handle, */
	bd->cmd_data[CMDIF_M_NAME_CHARS - 1] = '\0';
	bd->session_id = CMDIF_OPEN_SESSION;
	memcpy(&bd->cmd_data[CMDIF_M_NAME_CHARS], &handle, sizeof(handle));
	bd->cmdi_id = CMDIF_CMD_OPEN;

	return 0;
}

int cmdif_flib_open_done(struct cmdif_bd *bd_in_ring, uint16_t *session_id)
{
	if (bd_in_ring->cmdi_id != CMDIF_CMD_OPEN)
		return -EINVAL;
	if (!bd_in_ring->sync_done)
		return -EBUSY;

	*session_id = bd_in_ring->session_id;
	return 0;
}
