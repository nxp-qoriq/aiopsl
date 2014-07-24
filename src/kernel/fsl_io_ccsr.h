/**************************************************************************//**
 @File          fsl_io_ccsr.h

 @Description   CCSR I/O accessors
 *//***************************************************************************/
#ifndef __FSL_IO_CCSR_H
#define __FSL_IO_CCSR_H

#include "fsl_io.h"

/**************************************************************************//**
@Group		accessor_g Accessor API

 @Description   Endian Conversion functions to read/write with endianess treatment
 	 	 in CCSR

 @{
*//***************************************************************************/


#define ioread32_ccsr(addr)             ioread32be(addr)
#define iowrite32_ccsr(val, addr)       iowrite32be(val, addr)


/** @} */ /* end of accessor_g Accessor API group */

#endif /* __FSL_IO_CCSR_H */
