#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpmac.h>
#include <fsl_dpmac_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

int dpmac_mdio_read(struct dpmac *dpmac, struct dpmac_mdio_read_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPMAC_CMD_MDIO_READ(CMD_PREP);

	err = dplib_send(dpmac->regs, &(dpmac->auth), DPMAC_CMDID_MDIO_READ,
			 DPMAC_CMDSZ_MDIO_READ, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPMAC_RSP_MDIO_READ(RSP_READ_STRUCT);

	return err;
}

int dpmac_mdio_write(struct dpmac *dpmac, struct dpmac_mdio_write_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPMAC_CMD_MDIO_WRITE(CMD_PREP);

	return dplib_send(dpmac->regs, &(dpmac->auth), DPMAC_CMDID_MDIO_WRITE,
				DPMAC_CMDSZ_MDIO_WRITE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpmac_adjust_link(struct dpmac *dpmac, struct dpmac_adjust_link_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPMAC_CMD_ADJUST_LINK(CMD_PREP);

	return dplib_send(dpmac->regs, &(dpmac->auth), DPMAC_CMDID_ADJUST_LINK,
				DPMAC_CMDSZ_ADJUST_LINK, MC_CMD_PRI_LOW,
				&cmd_data);
}
