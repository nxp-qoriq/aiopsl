#include <fsl_cmdif_flib_s.h>
#include <cmdif_srv.h>
#include <errno.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>

#ifndef ENOTSUP
#define ENOTSUP		95	/*!< Operation not supported */
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT	110	/*!< Operation timed out */
#endif

#ifndef ENAVAIL
#define ENAVAIL		119	/*!< Resource not available, or not found */
#endif

#define UNUSED(_x)	((void)(_x))

#define FREE_MODULE    '\0'
#define FREE_INSTANCE  (M_NUM_OF_MODULES) 

static void my_memset(uint8_t *ptr, uint8_t val, uint32_t size)
{
	int i = 0;
	for (i = 0; i < size; i++) 
		ptr[i] = val;
}

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

	my_memset((uint8_t *)srv->m_name,
	          FREE_MODULE,
	          sizeof(srv->m_name[0]) * M_NUM_OF_MODULES);
	my_memset((uint8_t *)srv->inst_dev,
	          NULL,
	          sizeof(srv->inst_dev[0]) * M_NUM_OF_INSTANCES);
	my_memset(srv->m_id,
	          FREE_INSTANCE,
	          M_NUM_OF_INSTANCES);

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

static int empty_open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(instance_id);
	UNUSED(dev);
	return -ENODEV;
}

static int empty_close_cb(void *dev)
{
	UNUSED(dev);
	return -ENODEV;
}

static int empty_ctrl_cb(void *dev, uint16_t cmd, uint32_t size, uint64_t data)
{
	UNUSED(cmd);
	UNUSED(dev);
	UNUSED(size);
	UNUSED(data);
	return -ENODEV;
}

static int module_id_alloc( struct cmdif_srv *srv, const char *m_name, 
                            struct cmdif_module_ops *ops)
{
	int i = 0;
	int id = -ENAVAIL;

	if (m_name[0] == FREE_MODULE)
		return -EINVAL;


	for (i = 0; i < M_NUM_OF_MODULES; i++) {
		if ((srv->m_name[i][0] == FREE_MODULE) && (id < 0)) {
			id = i;
		} else if (strncmp(srv->m_name[i], m_name, M_NAME_CHARS) == 0) {
			return -EEXIST;
		}
	}
	if (id >= 0) {
		strncpy(srv->m_name[id], m_name, M_NAME_CHARS);
		srv->m_name[id][M_NAME_CHARS] = '\0';
		
		srv->ctrl_cb[id]  = empty_ctrl_cb;
		srv->open_cb[id]  = empty_open_cb;
		srv->close_cb[id] = empty_close_cb;

		if (ops->ctrl_cb)
			srv->ctrl_cb[id]  = ops->ctrl_cb;
		if (ops->open_cb)
			srv->open_cb[id]  = ops->open_cb;
		if (ops->close_cb)
			srv->close_cb[id] = ops->close_cb;
	}

	return id;
}

static int module_id_find(struct cmdif_srv *srv, const char *m_name)
{
	int i = 0;

	if (m_name[0] == FREE_MODULE)
		return -EINVAL;

	for (i = 0; i < M_NUM_OF_MODULES; i++) {
		if (strncmp(srv->m_name[i], m_name, M_NAME_CHARS) == 0)
			return i;
	}

	return -ENAVAIL;
}

int cmdif_srv_register(struct  cmdif_srv *srv, 
                       const char *m_name, 
                       struct cmdif_module_ops *ops)
{

	int    m_id = 0;

	if ((m_name == NULL) || (ops == NULL) || (srv == NULL))
		return -EINVAL;

	m_id = module_id_alloc(srv, m_name, ops);
	
	if (m_id < 0)
		return m_id;

	return 0;
}

int cmdif_srv_unregister(struct  cmdif_srv *srv, const char *m_name)
{
	int    m_id = -1;

	if ((m_name == NULL) || (srv == NULL))
		return -EINVAL;

	m_id = module_id_find(srv, m_name);
	if (m_id >= 0) {
		srv->ctrl_cb[m_id]   = NULL;
		srv->open_cb[m_id]   = NULL;
		srv->close_cb[m_id]  = NULL;
		srv->m_name[m_id][0] = FREE_MODULE;
		return 0;
	} else {
		return m_id; /* POSIX error is returned */
	}
}
