/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_enet.h

 @Description   TTODO
*//***************************************************************************/
#ifndef __FSL_ENET_H
#define __FSL_ENET_H

#include "common/types.h"


/**
 @Description  Ethernet MAC-PHY Interface
*/
enum enet_interface {
	FSL_ENET_IF_MII		= 0x00010000, /**< MII interface */
	FSL_ENET_IF_RMII		= 0x00020000, /**< RMII interface */
	FSL_ENET_IF_SMII		= 0x00030000, /**< SMII interface */
	FSL_ENET_IF_GMII		= 0x00040000, /**< GMII interface */
	FSL_ENET_IF_RGMII		= 0x00050000, /**< RGMII interface */
	FSL_ENET_IF_TBI		= 0x00060000, /**< TBI interface */
	FSL_ENET_IF_RTBI		= 0x00070000, /**< RTBI interface */
	FSL_ENET_IF_SGMII		= 0x00080000, /**< SGMII interface */
	FSL_ENET_IF_XGMII		= 0x00090000, /**< XGMII interface */
	FSL_ENET_IF_QSGMII	= 0x000a0000, /**< QSGMII interface */
	FSL_ENET_IF_XFI		= 0x000b0000  /**< XFI interface */
};

/**
 @Description  Ethernet Speed (nominal data rate)
*/
enum enet_speed {
	FSL_ENET_SPEED_10		= 10,	/**< 10 Mbps */
	FSL_ENET_SPEED_100	= 100,	/**< 100 Mbps */
	FSL_ENET_SPEED_1000	= 1000,	/**< 1000 Mbps = 1 Gbps */
	FSL_ENET_SPEED_10000	= 10000	/**< 10000 Mbps = 10 Gbps */
};

#define ENET_IF_SGMII_BASEX 0x80000000
	/**< SGMII/QSGII interface with 1000BaseX auto-negotiation between MAC
	and phy or backplane;
	Note: 1000BaseX auto-negotiation relates only to interface between MAC
	and phy/backplane, SGMII phy can still synchronize with far-end phy at
	10Mbps, 100Mbps or 1000Mbps */

enum enet_mode {
	FSL_ENET_MODE_INVALID           = 0,
		/**< Invalid Ethernet mode */
	FSL_ENET_MODE_MII_10            = (FSL_ENET_IF_MII   | FSL_ENET_SPEED_10),
		/**<    10 Mbps MII   */
	FSL_ENET_MODE_MII_100           = (FSL_ENET_IF_MII   | FSL_ENET_SPEED_100),
		/**<   100 Mbps MII   */
	FSL_ENET_MODE_RMII_10           = (FSL_ENET_IF_RMII  | FSL_ENET_SPEED_10),
		/**<    10 Mbps RMII  */
	FSL_ENET_MODE_RMII_100          = (FSL_ENET_IF_RMII  | FSL_ENET_SPEED_100),
		/**<   100 Mbps RMII  */
	FSL_ENET_MODE_SMII_10           = (FSL_ENET_IF_SMII  | FSL_ENET_SPEED_10),
		/**<    10 Mbps SMII  */
	FSL_ENET_MODE_SMII_100          = (FSL_ENET_IF_SMII  | FSL_ENET_SPEED_100),
		/**<   100 Mbps SMII  */
	FSL_ENET_MODE_GMII_1000         = (FSL_ENET_IF_GMII  | FSL_ENET_SPEED_1000),
		/**<  1000 Mbps GMII  */
	FSL_ENET_MODE_RGMII_10          = (FSL_ENET_IF_RGMII | FSL_ENET_SPEED_10),
		/**<    10 Mbps RGMII */
	FSL_ENET_MODE_RGMII_100         = (FSL_ENET_IF_RGMII | FSL_ENET_SPEED_100),
		/**<   100 Mbps RGMII */
	FSL_ENET_MODE_RGMII_1000        = (FSL_ENET_IF_RGMII | FSL_ENET_SPEED_1000),
		/**<  1000 Mbps RGMII */
	FSL_ENET_MODE_TBI_1000          = (FSL_ENET_IF_TBI   | FSL_ENET_SPEED_1000),
		/**<  1000 Mbps TBI   */
	FSL_ENET_MODE_RTBI_1000         = (FSL_ENET_IF_RTBI  | FSL_ENET_SPEED_1000),
		/**<  1000 Mbps RTBI  */
	FSL_ENET_MODE_SGMII_10          = (FSL_ENET_IF_SGMII | FSL_ENET_SPEED_10),
		/**< 10 Mbps SGMII with auto-negotiation between MAC and
		SGMII phy according to Cisco SGMII specification */
	FSL_ENET_MODE_SGMII_100         = (FSL_ENET_IF_SGMII | FSL_ENET_SPEED_100),
		/**< 100 Mbps SGMII with auto-negotiation between MAC and
		SGMII phy according to Cisco SGMII specification */
	FSL_ENET_MODE_SGMII_1000        = (FSL_ENET_IF_SGMII | FSL_ENET_SPEED_1000),
		/**< 1000 Mbps SGMII with auto-negotiation between MAC and
		SGMII phy according to Cisco SGMII specification */
	FSL_ENET_MODE_SGMII_BASEX_10    = (ENET_IF_SGMII_BASEX | FSL_ENET_IF_SGMII
		| FSL_ENET_SPEED_10),
		/**< 10 Mbps SGMII with 1000BaseX auto-negotiation between
		MAC and SGMII phy or backplane */
	FSL_ENET_MODE_SGMII_BASEX_100   = (ENET_IF_SGMII_BASEX | FSL_ENET_IF_SGMII
		| FSL_ENET_SPEED_100),
		/**< 100 Mbps SGMII with 1000BaseX auto-negotiation between
		MAC and SGMII phy or backplane */
	FSL_ENET_MODE_SGMII_BASEX_1000  = (ENET_IF_SGMII_BASEX | FSL_ENET_IF_SGMII
		| FSL_ENET_SPEED_1000),
		/**< 1000 Mbps SGMII with 1000BaseX auto-negotiation between
		MAC and SGMII phy or backplane */
	FSL_ENET_MODE_QSGMII_1000       = (FSL_ENET_IF_QSGMII | FSL_ENET_SPEED_1000),
		/**< 1000 Mbps QSGMII with auto-negotiation between MAC and
		QSGMII phy according to Cisco QSGMII specification */
	FSL_ENET_MODE_QSGMII_BASEX_1000 = (ENET_IF_SGMII_BASEX | FSL_ENET_IF_QSGMII
		| FSL_ENET_SPEED_1000),
		/**< 1000 Mbps QSGMII with 1000BaseX auto-negotiation between
		MAC and QSGMII phy or backplane */
	FSL_ENET_MODE_XGMII_10000       = (FSL_ENET_IF_XGMII | FSL_ENET_SPEED_10000),
		/**< 10000 Mbps XGMII */
	FSL_ENET_MODE_XFI_10000         = (FSL_ENET_IF_XFI   | FSL_ENET_SPEED_10000)
		/**< 10000 Mbps XFI */
};

#define _MAKFSL_ENET_MODE(_interface, _speed) (enum enet_mode)((_interface) \
	| (_speed))

#define _ENET_INTERFACE_FROM_MODE(mode) (enum enet_interface) \
	((mode) & 0x0fff0000)
#define _ENET_SPEED_FROM_MODE(mode) (enum enet_speed)((mode) & 0x0000ffff)
#define _ENET_ADDR_TO_UINT64(_enet_addr)		\
	(uint64_t)(((uint64_t)(_enet_addr)[0] << 40) |	\
		((uint64_t)(_enet_addr)[1] << 32) |	\
		((uint64_t)(_enet_addr)[2] << 24) |	\
		((uint64_t)(_enet_addr)[3] << 16) |	\
		((uint64_t)(_enet_addr)[4] << 8) |	\
		((uint64_t)(_enet_addr)[5]))

#define _MAKFSL_ENET_ADDR_FROM_UINT64(_addr64, _enet_addr)		\
	do {								\
		int i;							\
		for (i = 0; i < ENET_NUM_OCTETS_PER_ADDRESS; i++)	\
			(_enet_addr)[i] = (uint8_t)((_addr64) >> ((5-i)*8));\
	} while (0)

#endif /* __FSL_ENET_H */
