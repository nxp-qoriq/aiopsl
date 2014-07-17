/**************************************************************************//**
 @File          types.h

 @Description   TODO
 *//***************************************************************************/
#ifndef __FSL_TYPES_H
#define __FSL_TYPES_H

#if defined(__GNUC__) && defined(__cplusplus)
#include "kernel/types_gpp.h"

#elif defined(__GNUC__)
#include "kernel/types_gcc.h"

#elif defined(__MWERKS__)
#include "kernel/types_mw.h"

#else
#error "missing types definition"
#endif


/**************************************************************************//**
 @Description   General Handle
 *//***************************************************************************/
typedef void * fsl_handle_t; /**< TODO: remove, do not use */


#endif /* __FSL_TYPES_H */
