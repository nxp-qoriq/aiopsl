#include <fsl_dplib_sys.h>
#include <fsl_cmdif.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpmac.h>
#include <fsl_dpmac_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
do { \
	cmd_data.params[_param] |= u64_enc(_offset, _width, _arg); \
}while (0);

#define RSP_READ(_param, _offset, _width, _type, _arg) \
do { \
	*(_arg) = (_type)u64_dec(cmd_data.params[_param], _offset, _width);\
}while (0);	

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
do{\
	_arg = (_type)u64_dec(cmd_data.params[_param], _offset, _width);\
}while (0);

int dpmac_mdio_read(struct dpmac *dpmac, struct dpmac_mdio_read_cfg *cfg)
{
	struct mc_cmd_data cmd_data = {{ 0 }};
	int err;
	/* prepare command */
	DPMAC_CMD_MDIO_READ(CMD_PREP);

	err = cmdif_send(&(dpmac->cidesc), DPMAC_CMDID_MDIO_READ,
	                  DPMAC_CMDSZ_MDIO_READ, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err)
		DPMAC_RSP_MDIO_READ(RSP_READ_STRUCT);
	return err;
}

int dpmac_mdio_write(struct dpmac *dpmac, struct dpmac_mdio_write_cfg *cfg)
{
	struct mc_cmd_data cmd_data = {{ 0 }};

	/* prepare command */
	DPMAC_CMD_MDIO_WRITE(CMD_PREP);

	return cmdif_send(&(dpmac->cidesc), DPMAC_CMDID_MDIO_WRITE,
	                  DPMAC_CMDSZ_MDIO_WRITE, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
}

int dpmac_adjust_link(struct dpmac *dpmac, struct dpmac_adjust_link_cfg *cfg)
{
	struct mc_cmd_data cmd_data = {{ 0 }};

	/* prepare command */
	DPMAC_CMD_ADJUST_LINK(CMD_PREP);

	return cmdif_send(&(dpmac->cidesc), DPMAC_CMDID_ADJUST_LINK,
	                  DPMAC_CMDSZ_ADJUST_LINK, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
}
