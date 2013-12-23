/**************************************************************************//*
 @File          dprc.h

 @Description   DPRC FLib internal file

 @Cautions      None.
 *//***************************************************************************/

#ifndef _DPRC_H
#define _DPRC_H

#include "common\fsl_cmdif.h"
#include "dplib\fsl_dprc.h"

struct dprc {
	struct cmdif_dev *dev;
};

#endif /* _DPRC_H */
