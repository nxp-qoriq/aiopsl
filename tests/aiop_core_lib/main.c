#include "fsl_ldpaa.h"
#include "general.h"
#include "fsl_ste.h"
#include "fsl_tman.h"
#include "fsl_ctlu.h"
#include "fsl_fdma.h"
#include "fsl_gso.h"
#include "fsl_gro.h"
#include "fsl_ipf.h"
#include "fsl_ipr.h"
#include "fsl_ipsec.h"
#include "fsl_osm.h"
#include "fsl_parser.h"
#include "fsl_ste.h"
#include "fsl_tman.h"
#include "fsl_cdma.h"
#include "fsl_ipv4_checksum.h"
#include "fsl_l4_checksum.h"
#include "aiop_verification.h"

#define __wait()	asm ("wait  \n"	)

int main()
{	
	__wait();
	aiop_verification();
	return 0;
}
