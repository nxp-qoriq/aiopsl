#ifndef __FSL_MC_CMDIF_H
#define __FSL_MC_CMDIF_H

#define MC_CMD_NUM_OF_PARAMS	7

struct mc_cmd_data {
	uint64_t params[MC_CMD_NUM_OF_PARAMS];
};

struct mc_portal {
	uint64_t header;
	struct mc_cmd_data data;
};

enum mc_cmd_status {
	MC_CMD_STATUS_OK = 0x0, /**< passed */
	MC_CMD_STATUS_READY = 0x1, /**< Ready to be processed */
	MC_CMD_STATUS_AUTH_ERR = 0x3, /**< Authentication error */
	MC_CMD_STATUS_NO_PRIVILEGE = 0x4,
	MC_CMD_STATUS_DMA_ERR = 0x5,
	MC_CMD_STATUS_CONFIG_ERR = 0x6,
	MC_CMD_STATUS_TIMEOUT = 0x7,
	MC_CMD_STATUS_NO_RESOURCE = 0x8,
	MC_CMD_STATUS_NO_MEMORY = 0x9,
	MC_CMD_STATUS_BUSY = 0xA,
	MC_CMD_STATUS_UNSUPPORTED_OP = 0xB,
	MC_CMD_STATUS_INVALID_STATE = 0xC
};

#define MC_CMD_HDR_CMDID_O	52	/* Command ID field offset */
#define MC_CMD_HDR_CMDID_S	12	/* Command ID field size */
#define MC_CMD_HDR_AUTHID_O	38	/* Authentication ID field offset */
#define MC_CMD_HDR_AUTHID_S	10	/* Authentication ID field size */
#define MC_CMD_HDR_SIZE_O	31	/* Size field offset */
#define MC_CMD_HDR_SIZE_S	6	/* Size field size */
#define MC_CMD_HDR_STATUS_O	16	/* Status field offset */
#define MC_CMD_HDR_STATUS_S	8	/* Status field size*/
#define MC_CMD_HDR_PRI_O	15	/* Priority field offset */
#define MC_CMD_HDR_PRI_S	1	/* Priority field size */

#define MC_CMD_HDR_READ_STATUS(_hdr) \
	((enum mc_cmd_status)u64_dec((_hdr), \
		MC_CMD_HDR_STATUS_O, MC_CMD_HDR_STATUS_S))

#define MC_CMD_HDR_READ_AUTHID(_hdr) \
	((uint16_t)u64_dec((_hdr), MC_CMD_HDR_AUTHID_O, MC_CMD_HDR_AUTHID_S))

#define MC_CMDID_CLOSE		0x800
#define MC_DPNI_CMDID_OPEN	0x801
#define MC_DPSW_CMDID_OPEN	0x802
#define MC_DPIO_CMDID_OPEN	0x803
#define MC_DPBP_CMDID_OPEN	0x804
#define MC_DPRC_CMDID_OPEN	0x805
#define MC_DPDMUX_CMDID_OPEN	0x806
#define MC_DPCI_CMDID_OPEN	0x807
#define MC_DPCON_CMDID_OPEN	0x808
#define MC_DPSECI_CMDID_OPEN 0x809


#define MC_DPNI_CMDID_CREATE	0x901
#define MC_DPSW_CMDID_CREATE	0x902
#define MC_DPIO_CMDID_CREATE	0x903
#define MC_DPBP_CMDID_CREATE	0x904
#define MC_DPRC_CMDID_CREATE	0x905
#define MC_DPDMUX_CMDID_CREATE	0x906
#define MC_DPCI_CMDID_CREATE    0x907
#define MC_DPCON_CMDID_CREATE   0x908
#define MC_DPSECI_CMDID_CREATE  0x909

#define MC_CMD_OPEN_SIZE	8
#define MC_CMD_CLOSE_SIZE	0

#define MC_OPT_CMD_CREATE	1

static inline void mc_cmd_write(struct mc_portal *portal,
	uint16_t cmd_id,
	uint16_t auth_id,
	uint8_t size,
	int pri,
	struct mc_cmd_data *cmd_data)
{
	uint64_t hdr;
	int i;

	hdr = u64_enc(MC_CMD_HDR_CMDID_O, MC_CMD_HDR_CMDID_S, cmd_id);
	hdr |= u64_enc(MC_CMD_HDR_AUTHID_O, MC_CMD_HDR_AUTHID_S, auth_id);
	hdr |= u64_enc(MC_CMD_HDR_SIZE_O, MC_CMD_HDR_SIZE_S, size);
	hdr |= u64_enc(MC_CMD_HDR_PRI_O, MC_CMD_HDR_PRI_S, pri);
	hdr |= u64_enc(MC_CMD_HDR_STATUS_O, MC_CMD_HDR_STATUS_S,
			MC_CMD_STATUS_READY);

	if (cmd_data)
		/* copy command parameters into the portal */
		for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
			iowrite64(cmd_data->params[i],
					&(portal->data.params[i]));
	else
		/* zero all parameters (optional, consider skipping it) */
		for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
			iowrite64(0, &(portal->data.params[i]));

	/* submit the command by writing the header */
	iowrite64(hdr, &portal->header);
}

static inline enum mc_cmd_status mc_cmd_read_status(struct mc_portal *portal)
{
	uint64_t hdr = ioread64(&(portal->header));

	return MC_CMD_HDR_READ_STATUS(hdr);
}

static inline uint16_t mc_cmd_read_auth_id(struct mc_portal *portal)
{
	uint64_t hdr = ioread64(&(portal->header));

	return MC_CMD_HDR_READ_AUTHID(hdr);
}

static inline void mc_cmd_read_response(struct mc_portal *portal,
	struct mc_cmd_data *resp)
{
	int i;

	if (resp)
		for (i = 0; i < MC_CMD_NUM_OF_PARAMS; i++)
			resp->params[i] = ioread64(&(portal->data.params[i]));
}

#endif /* __FSL_MC_CMDIF_H */
