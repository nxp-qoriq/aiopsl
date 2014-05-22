#include <fsl_cmdif_flib.h>
#include <cmdif_srv.h>
#include <errno.h>
#include <types.h>
#include <string.h>

#define FREE_MODULE    '\0'
#define TAKEN_INSTANCE (void *)0xFFFFFFFF
#define FREE_INSTANCE  NULL

struct cmdif_srv *cmdif_srv_allocate(void *(*fast_malloc)(int size),
				void *(*slow_malloc)(int size))
{
	struct cmdif_srv *srv = fast_malloc(sizeof(struct cmdif_srv));

	if (srv == NULL) {
		return NULL;
	}

	/* SHRAM */
	srv->inst_dev  = fast_malloc(sizeof(void *) * M_NUM_OF_INSTANCES);
	srv->m_id      = fast_malloc(M_NUM_OF_INSTANCES);
	srv->ctrl_cb   = fast_malloc(sizeof(ctrl_cb_t *) * M_NUM_OF_MODULES);
	srv->sync_done = fast_malloc(sizeof(void *) * M_NUM_OF_INSTANCES);
	/* DDR */
	srv->m_name    = slow_malloc(sizeof(char[M_NAME_CHARS + 1]) * \
				M_NUM_OF_MODULES);
	srv->open_cb   = slow_malloc(sizeof(open_cb_t *) * M_NUM_OF_MODULES);
	srv->close_cb  = slow_malloc(sizeof(close_cb_t *) * M_NUM_OF_MODULES);

	if ((srv->inst_dev == NULL)     || (srv->m_id == NULL)      ||
		(srv->ctrl_cb == NULL)  || (srv->sync_done == NULL) ||
		(srv->m_name == NULL)   || (srv->open_cb == NULL)   ||
		(srv->close_cb == NULL)) {
		return NULL;
	}

	memset(srv->m_name,
	       FREE_MODULE,
	       sizeof(srv->m_name[0]) * M_NUM_OF_MODULES);
	memset(srv->inst_dev,
	       FREE_INSTANCE,
	       sizeof(srv->inst_dev[0]) * M_NUM_OF_INSTANCES);
	srv->inst_count = 0;

	return srv;
}

void cmdif_srv_deallocate(struct  cmdif_srv *srv, void (*free)(void *ptr))
{
	if (srv != NULL) {
		if (srv->inst_dev)
			free(srv->inst_dev);
		if (srv->m_id)
			free(srv->m_id);
		if (srv->sync_done)
			free(srv->sync_done);
		if (srv->m_name)
			free(srv->m_name);
		if (srv->open_cb)
			free(srv->open_cb);
		if (srv->ctrl_cb)
			free(srv->ctrl_cb);
		if (srv->open_cb)
			free(srv->open_cb);

		free(srv);
	}
}
