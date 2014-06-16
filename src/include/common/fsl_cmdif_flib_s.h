#ifndef __FSL_CMDIF_FLIB_H
#define __FSL_CMDIF_FLIB_H

#include <types.h>
#include <fsl_cmdif_server.h>
#include <fsl_cmdif_fd.h>

/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_cmdif_flib_s.h
 *  @brief   Cmdif AIOP<->GPP FLIB header file for server
 */

/**
 *
 * @brief	Allocate server handle to be used by server FLIBs.
 *
 * Should be used one time during server initialization.
 *
 * @param[in]	fast_malloc  - Malloc function for fast memory allocation that
 *                             is accessed for every command.
 * @param[in]	slow_malloc  - Malloc function for slow memory allocation,
 *                             this memory will be used to malloc data that is
 *                             accessed only during initialization.
 *
 * @returns	Valid pointer on success; NULL otherwise.
 *
 */
void *cmdif_srv_allocate(void *(*fast_malloc)(int),
			void *(*slow_malloc)(int));

/**
 *
 * @brief	Deallocate server handle allocated by cmdif_srv_allocate().
 *
 * Should be used one time during server shutdown.
 *
 * @param[in]	srv          - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	free         - Function to be used to free server allocated
 *                             memory.
 * @returns	None.
 *
 */
void cmdif_srv_deallocate(void *srv,
			void (*free)(void *ptr));

/**
 *
 * @brief	Unregister module under server.
 *
 * Should be used to implement cmdif_unregister_module.
 * This function is not multitask protected.
 * Wrap it with locks if required.
 *
 * @param[in]	srv          - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	m_name       - Module name to unregister
 *
 * @returns	0 on success, error code otherwise.
 *
 */
int cmdif_srv_unregister(void *srv,
			const char *m_name);

/**
 *
 * @brief	Register module under server.
 *
 * Should be used to implement cmdif_register_module.
 * This function is not multitask protected.
 * Wrap it with locks if required.
 *
 * @param[in]	srv          - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	m_name       - Module name to unregister
 * @param[in]	ops          - Module callback functions
 *
 * @returns	0 on success, error code otherwise.
 *
 */
int cmdif_srv_register(void *srv,
		const char *m_name,
		struct cmdif_module_ops *ops);

/**
 *
 * @brief	Open session on server
 *
 * Should be used for implementation of cmdif_session_open()
 * or inside cmdif_srv_cb().
 * This API is to be used to create a session on server.
 * Session information will be placed inside v_data, this buffer can be send to
 * the other side using #CMD_ID_NOTIFY_OPEN command.
 *
 * @param[in]	srv      - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	m_name   - Name of the module that have been registered using
 *                         cmdif_srv_register()
 * @param[in]	inst_id  - Instance id which will be passed to #open_cb_t
 * @param[in]	size     - Size of v_data buffer.
 * @param[in]	dev_id   - Transport device id to be used for this session.
 * @param[out]	v_data   - Buffer allocated by user.
 *                         If not NULL this buffer will carry all
 *                         the information of this session.
 * @param[out]	auth_id  - Session id as returned by server.
 *
 * @returns	O on success or error code otherwise.
 */
int cmdif_srv_open(void *srv,
		const char *m_name,
		uint8_t inst_id,
		uint32_t dev_id,
		uint32_t size,
		void *v_data,
		uint16_t *auth_id);

/**
 *
 * @brief	Close session on server
 *
 * Should be used for implementation of cmdif_session_close()
 * or inside cmdif_srv_cb().
 * This API is to be used to close a session on server.
 * Session information will be placed inside v_data, this buffer can be send to
 * the other side using #CMD_ID_NOTIFY_CLOSE command.
 *
 * @param[in]	srv      - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	auth_id  - Session id as returned by cmdif_srv_open().
 * @param[in]	dev_id   - Transport device id to be used for this session.
 * @param[in]	size     - Size of v_data buffer.
 * @param[out]	v_data   - Buffer allocated by user.
 *                         If not NULL this buffer will carry all
 *                         the information of this session.
 *
 * @returns	O on success or error code otherwise.
 */
int cmdif_srv_close(void *srv,
		uint16_t auth_id,
		uint32_t dev_id,
		uint32_t size,
		void *v_data);

/**
 *
 * @brief	Server handle command function
 *
 * Should be called upon every command frame that have been dequeued.
 * Use it inside cmdif_srv_cb()
 *
 * @param[in]	srv       - Server handle allocated by cmdif_srv_allocate()
 * @param[in]	cfd       - CMDIF input frame descriptor
 * @param[out]	cfd_out   - CMDIF output frame descriptor,
 *                          if response is required
 * @param[out]	send_resp - Response indication. If set to 1 the response FD
 *                          must be sent.
 *
 * @returns	O on success or error code otherwise.
 */
int cmdif_srv_cmd(void *srv,
		struct cmdif_fd *cfd,
		struct cmdif_fd *cfd_out,
		uint8_t *send_resp);

#endif /* __FSL_CMDIF_FLIB_H */
