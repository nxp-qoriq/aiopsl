#ifndef __FSL_CMDIF_FLIB_H
#define __FSL_CMDIF_FLIB_H

#include <types.h>
#include <fsl_cmdif_server.h>

/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_cmdif_flib_s.h
 *  @brief   Cmdif AIOP<->GPP FLIB header file for server
 */

struct cmdif_srv *cmdif_srv_allocate(void *(*fast_malloc)(int),
				     void *(*slow_malloc)(int));
void cmdif_srv_deallocate(struct  cmdif_srv *srv, 
                          void (*free)(void *ptr));
int cmdif_srv_unregister(struct  cmdif_srv *srv, 
                         const char *m_name);
int cmdif_srv_register(struct  cmdif_srv *srv, 
                       const char *m_name, 
                       struct cmdif_module_ops *ops);

#endif /* __FSL_CMDIF_FLIB_H */
