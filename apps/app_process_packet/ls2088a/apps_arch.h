#ifndef __APPS_ARCH_H
#define __APPS_ARCH_H

#define ARCH_DP_DDR_SIZE	(128 * MEGABYTE)
#define ARCH_CTLU_DP_DDR_NUM_ENTRIES (2048)
#define ARCH_MFLU_DP_DDR_NUM_ENTRIES (2048)


/* fdma_discard_fd API is different for rev1 and rev2 */
#define ARCH_FDMA_DISCARD_FD() \
	fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS, 0, FDMA_DIS_AS_BIT);

#endif /* __APPS_ARCH_H */
