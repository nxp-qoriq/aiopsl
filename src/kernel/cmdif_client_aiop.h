
/*!
 *  @file    cmdif_client.h
 *  @brief   Cmdif client AIOP<->GPP internal header file
 */

#ifndef __CMDIF_CLIENT_AIOP_H
#define __CMDIF_CLIENT_AIOP_H

#include "cmdif_client.h"
#include "dplib/fsl_dpci.h"

/** BDI */
#define BDI_GET \
((((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fdsrc_va_fca_bdi) \
	& ADC_BDI_MASK)
/** VA
 * TODO is it really OR between VA in ADC and FD */
#define VA_GET \
(((((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fdsrc_va_fca_bdi) \
	& ADC_VA_MASK) || LDPAA_FD_GET_VA(HWC_FD_ADDRESS))

/** BMT for memory accesses */
#define BMT_GET \
	LDPAA_FD_GET_CBMT(HWC_FD_ADDRESS)

/** PL_ICID from Additional Dequeue Context */
#define PL_ICID_GET \
	LH_SWAP(0, &(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->pl_icid))

/** Get ICID to send response */
#define ICID_GET(PL_AND_ICID) ((PL_AND_ICID) & ADC_ICID_MASK)

/** Get PL to send response */
#define PL_GET(PL_AND_ICID) ((PL_AND_ICID)  & ADC_PL_MASK)

#define ADD_AMQ_FLAGS(FL, PL_AND_ICID)		\
	do {					\
		if (PL_GET(PL_AND_ICID))	\
			FL |= FDMA_DMA_PL_BIT;	\
		if (VA_GET)			\
			FL |= FDMA_DMA_VA_BIT;	\
		if (BMT_GET)			\
			FL |= FDMA_DMA_BMT_BIT;	\
	}while(0)

#define CMDIF_MN_SESSIONS	64 /**< Maximal number of sessions */
#define CMDIF_NUM_PR  		2

struct cmdif_reg {
	struct dpci *dpci_dev;
	/**< Open AIOP dpci device */
	struct dpci_attr *attr;
};

/* To be allocated on DDR */
struct cmdif_cl {
	struct {
		char m_name[M_NAME_CHARS + 1];
		/**< Module application name */
		struct cmdif_reg *regs;
		/**< Send device, to be placed as cidesc.reg */
		struct cmdif_dev *dev;
		/**< To be placed as cidesc.dev */
		uint8_t ins_id;
		/**< Instanse id that was used for open */
	} gpp[CMDIF_MN_SESSIONS];

	uint8_t count;
	/**< Count the number of sessions */
	uint8_t lock;
	/**< Lock for adding & removing new entries */
};


#endif /* __CMDIF_CLIENT_H */
