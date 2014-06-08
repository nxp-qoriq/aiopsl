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

struct cmdif_srv;

struct cmdif_srv *cmdif_srv_allocate(void *(*fast_malloc)(int),
				     void *(*slow_malloc)(int));

void cmdif_srv_deallocate(struct cmdif_srv *srv, 
                          void (*free)(void *ptr));

int cmdif_srv_unregister(struct  cmdif_srv *srv, 
                         const char *m_name);

int cmdif_srv_register(struct cmdif_srv *srv, 
                       const char *m_name, 
                       struct cmdif_module_ops *ops);

/**
 *
 * @brief	Open command initiated by server
 *
 * Should be used for implementation of cmdif_session_open().
 *
 * @param[in]	num_pr      - Number of priorities, it defined the number of
 *                            DPCI queues to be used for this direction;
 *                            The queues to be picked will be from the high
 *                            to low indexes inside DPCI.
 *
 * @returns	Error code 
 */
int cmdif_srv_open(struct cmdif_srv *srv, 
                   const char *m_name, 
                   uint8_t inst_id, 
                   void * v_data, 
                   uint64_t p_data, 
                   uint32_t size, 
                   uint16_t *auth_id,
                   uint8_t num_pr);

int cmdif_srv_close(struct cmdif_srv *srv, 
                    uint16_t auth_id, 
                    uint64_t *p_data, 
                    void **v_data, 
                    int *size);

int cmdif_srv_cmd(struct cmdif_srv *srv, 
                  struct cmdif_fd *cfd, 
                  struct cmdif_fd *cfd_out);

#endif /* __FSL_CMDIF_FLIB_H */
