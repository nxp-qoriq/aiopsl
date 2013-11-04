/**************************************************************************//*
 @File          dpni.h

 @Description   DPNI FLib internal file

 @Cautions      None.
*//***************************************************************************/

#ifndef _DPNI_H
#define _DPNI_H

#include "common\fsl_cmdif.h"
#include "dplib\fsl_dpni.h"


/**************************************************************************//**
 @Description   FLib internal dataBase

                more_detailed_description
*//***************************************************************************/
struct dpni
{
	struct cmdif_dev	*dev;
    int     			taken;			/**< Indicate if resource is in use */
};


#endif /* _DPNI_H */
