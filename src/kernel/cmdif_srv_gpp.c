#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "general.h"
#include "io.h"
#include "fsl_malloc.h"
#include "errors.h"
#include "cmdif_srv.h"
#include "fsl_cmdif_flib_s.h"
#include "fsl_cmdif_client.h"

void *srv;

static void *fast_malloc(int size)
{
	return NULL;
}

static void *slow_malloc(int size)
{
	return NULL;
}

static void srv_free(void *ptr)
{
	if (ptr != NULL)
		fsl_os_xfree(ptr);
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

static int send_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{
	/* TODO complete */
	return 0;
}

int cmdif_srv_cb(struct cmdif_fd *cfd, int pr, void *send_dev)
{
	int    err = 0;
	struct cmdif_fd  cfd_out;

	if (srv == NULL)
		return -ENODEV;


	/* Call ctrl cb; if no perm cfd_out will be invalid */
	err = cmdif_srv_cmd(srv, cfd, &cfd_out);
	/* don't bother to send response
	 * in order not to overload response queue,
	 * it might be intentional attack
	 * */
	if (err == -EPERM)
		return err;
	
	err = send_fd(&cfd_out, pr, send_dev);
	
	return err;
}

int cmdif_session_open(struct cmdif_desc *cidesc,
                       const char *m_name,
                       uint8_t inst_id,
                       void *v_data,
                       uint64_t p_data,
                       uint32_t size,
                       uint16_t *auth_id)
{
	int      err = 0;
	
	/*Call open_cb , Store dev */
	err = cmdif_srv_open(srv, m_name, inst_id, v_data, 
	                     size, auth_id, dpci_id);
	
	/*Send information to AIOP */
	err = cmdif_send(cidesc, CMD_ID_NOTIFY_OPEN, size, CMDIF_PRI_LOW,
	                 p_data);
		
	return err;	
}

int cmdif_session_close(struct cmdif_desc *cidesc,
                        uint16_t auth_id,
                        void *v_data,
                        uint64_t p_data,
                        uint32_t size)
{
	int      err = 0;
	uint16_t auth_id = 0;
	
	/*Call close_cb , place dpci_id, auth_id inside p_data */
	err = cmdif_srv_close(srv, auth_id, v_data, size);
	
	/*Send information to AIOP */
	err = cmdif_send(cidesc, CMD_ID_NOTIFY_CLOSE, size, CMDIF_PRI_LOW, 
	                 p_data);
		
	return err;	
}
