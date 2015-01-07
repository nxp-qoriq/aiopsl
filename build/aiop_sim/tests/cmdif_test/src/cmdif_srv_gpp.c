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

#include <errno.h>
#include <types.h>
#include <fsl_cmdif_flib_s.h>
#include <fsl_cmdif_client.h>

int cmdif_srv_init(void);
void cmdif_srv_free(void);

void *srv;

static void *fast_malloc(int size)
{
	/* TODO */
	return NULL;
}

static void *slow_malloc(int size)
{
	/* TODO */
	return NULL;
}

static uint64_t virt_to_phys(void *ptr)
{
	/* TODO */
	return 0;
}

static void srv_free(void *ptr)
{
	/* TODO */
	return;
}


static int receive_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{
	/* Find queue according to priority  */
	/* TODO vq = find_queue(sdev, pr); */

	/* Get FD */
	/* TODO nadk_receive(sdev, vq, &nbuf, 1); */

	/* TODO Copy FD fields from NADK buff
	 * cfd->u_frc.frc;
	 * cfd->d_size;
	 * cfd->u_flc.flc;
	 * cfd->u_addr.addr;
	 * Example:
	 * cfd->d_size = nbuf.length;
	 * */

	return 0;
}

static int send_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{

	/* TODO Copy FD fields into NADK buff
	 * cfd->u_frc.frc;
	 * cfd->d_size;
	 * cfd->u_flc.flc;
	 * cfd->u_addr.addr;
	 * Example:
	 * nbuf.length = cfd->d_size;
	 * */

	/* Find queue according to priority  */
	/* TODO vq = find_queue(sdev, pr); */

	/* Get FD */
	/* TODO nadk_receive(sdev, vq, &nbuf, 1); */

	return 0;
}

int cmdif_srv_init(void)
{
	int     err = 0;

	srv = cmdif_srv_allocate(fast_malloc, slow_malloc);
	if (srv == NULL)
		return -ENOMEM;

	return err;
}

void cmdif_srv_free(void)
{
	cmdif_srv_deallocate(srv, srv_free);
}

int cmdif_register_module(const char *m_name, struct cmdif_module_ops *ops)
{
	/* Place here lock if required */

	return cmdif_srv_register(srv, m_name, ops);
}

int cmdif_unregister_module(const char *m_name)
{
	/* Place here lock if required */

	return cmdif_srv_unregister(srv, m_name);
}

int cmdif_srv_cb(int pr, void *send_dev)
{
	int     err = 0;
	struct  cmdif_fd cfd_out;
	struct  cmdif_fd cfd;
	uint8_t send_resp = 0;

	if (srv == NULL)
		return -ENODEV;


	err = receive_fd(&cfd, pr, send_dev);

	/* Call ctrl cb; if no perm cfd_out will be invalid */
	err = cmdif_srv_cmd(srv, &cfd, &cfd_out, &send_resp);
	/* don't bother to send response
	 * in order not to overload response queue,
	 * it might be intentional attack
	 * */
	if (err == -EPERM)
		return err;

	if (send_resp)
		err = send_fd(&cfd_out, pr, send_dev);

	return err;
}

int cmdif_session_open(struct cmdif_desc *cidesc,
		const char *m_name,
		uint8_t inst_id,
		uint32_t size,
		void *v_data,
		uint16_t *auth_id)
{
	int      err = 0;
	uint32_t dpci_id = 0; /* TODO Get DPCI id of GPP server */
	uint64_t p_data  = virt_to_phys(v_data);

	/* Place here lock if required */

	/*Call open_cb , Store dev */
	err = cmdif_srv_open(srv, m_name, inst_id, dpci_id, size, v_data,
			auth_id);

	/*Send information to AIOP */
	err = cmdif_send(cidesc, CMD_ID_NOTIFY_OPEN, size, CMDIF_PRI_LOW,
			p_data);

	return err;
}

int cmdif_session_close(struct cmdif_desc *cidesc,
			uint16_t auth_id,
			uint32_t size,
			void *v_data)
{
	int      err = 0;
	uint32_t dpci_id = 0; /* TODO Get DPCI id of GPP server */
	uint64_t p_data  = virt_to_phys(v_data);

	/* Place here lock if required */

	/*Call close_cb , place dpci_id, auth_id inside p_data */
	err = cmdif_srv_close(srv, auth_id, dpci_id, size, v_data);

	/*Send information to AIOP */
	err = cmdif_send(cidesc, CMD_ID_NOTIFY_CLOSE, size, CMDIF_PRI_LOW,
			p_data);

	return err;
}
