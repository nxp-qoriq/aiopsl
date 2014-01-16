
#include <fsl_dplib_sys.h>
#include <fsl_cmdif.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dprc.h>
#include <fsl_dprc_cmd.h>


#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	cmd_data.params[_param] |= u64_enc(_offset, _width, _arg);

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	*(_arg) = (_type)u64_dec(cmd_data.params[_param], _offset, _width);

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	_arg = (_type)u64_dec(cmd_data.params[_param], _offset, _width);

int dprc_get_container_id(struct dprc *dprc, int *container_id)
{
	/*TODO - review*/
	struct mc_cmd_data cmd_data = { 0 };
	int err;

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_CONT_ID,
	                 DPRC_CMDSZ_GET_CONT_ID, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_CONTAINER_ID(RSP_READ);
	}
	return err;
}

int dprc_open(struct dprc *dprc, int container_id)
{
	return cmdif_open(&(dprc->cidesc), CMDIF_MOD_DPRC,
	                  (uint16_t)container_id);
}

int dprc_close(struct dprc *dprc)
{
	return cmdif_close(&(dprc->cidesc));
}

int dprc_create_container(struct dprc *dprc,
                          struct dprc_create_attributes *attr,
                          int *child_container_id,
                          uint64_t *child_portal_paddr)
{
	struct mc_cmd_data cmd_data = { 0 };
	int err;

	/* prepare command */
	DPRC_CMD_CREATE_CONTAINER(CMD_PREP);

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_CREATE_CONT,
	                 DPRC_CMDSZ_CREATE_CONT, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_CREATE_CONTAINER(RSP_READ);
	}
	return err;
}

int dprc_destroy_container(struct dprc *dprc, int child_container_id)
{
	struct mc_cmd_data cmd_data = { 0 };

	/* prepare command */
	DPRC_CMD_DESTROY_CONTAINER(CMD_PREP);

	return cmdif_send(&(dprc->cidesc), DPRC_CMDID_DESTROY_CONT,
	                  DPRC_CMDSZ_DESTROY_CONT, CMDIF_PRI_LOW,
	                  (uint8_t*)&cmd_data);
}

int dprc_reset_container(struct dprc *dprc, int child_container_id)
{
	struct mc_cmd_data cmd_data = { 0 };

	/* prepare command */
	DPRC_CMD_RESET_CONTAINER(CMD_PREP);

	return cmdif_send(&(dprc->cidesc), DPRC_CMDID_RESET_CONT,
	                  DPRC_CMDSZ_RESET_CONT, CMDIF_PRI_LOW,
	                  (uint8_t*)&cmd_data);
}

int dprc_set_res_quota(struct dprc *dprc,
                       int child_container_id,
                       uint16_t res_type,
                       uint16_t quota)
{
	struct mc_cmd_data cmd_data = { 0 };
	uint16_t res_type_def = res_type;
	/* prepare command */
	DPRC_CMD_SET_RES_QUOTA(CMD_PREP);

	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc), DPRC_CMDID_SET_RES_QUOTA,
	                  DPRC_CMDSZ_SET_RES_QUOTA, CMDIF_PRI_LOW,
	                  (uint8_t*)&cmd_data);
}

int dprc_get_res_quota(struct dprc *dprc,
                       int child_container_id,
                       uint16_t res_type,
                       uint16_t *quota)
{
	struct mc_cmd_data cmd_data = { 0 };
	int err;
	uint16_t res_type_def = res_type;
	/* prepare command */
	DPRC_CMD_GET_RES_QUOTA(CMD_PREP);

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_RES_QUOTA,
	                 DPRC_CMDSZ_GET_RES_QUOTA, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_RES_QUOTA(RSP_READ);
	}

	return err;
}

int dprc_assign(struct dprc *dprc,
                int container_id,
                struct dprc_res_req *res_req)
{
	struct mc_cmd_data cmd_data = { 0 };
	uint16_t type_def = res_req->type;
	DPRC_CMD_ASSIGN(CMD_PREP);

	return cmdif_send(&(dprc->cidesc), DPRC_CMDID_ASSIGN, DPRC_CMDSZ_ASSIGN,
	                  CMDIF_PRI_LOW, (uint8_t*)&cmd_data);
}

int dprc_unassign(struct dprc *dprc,
                  int child_container_id,
                  struct dprc_res_req *res_req)
{
	struct mc_cmd_data cmd_data = { 0 };
	uint16_t type_def = res_req->type;
	DPRC_CMD_UNASSIGN(CMD_PREP);

	return cmdif_send(&(dprc->cidesc), DPRC_CMDID_UNASSIGN,
	                  DPRC_CMDSZ_UNASSIGN, CMDIF_PRI_LOW,
	                  (uint8_t*)&cmd_data);
}

int dprc_get_device_count(struct dprc *dprc, int *dev_count)
{
	struct mc_cmd_data cmd_data = { 0 };
	int err;

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_DEV_COUNT,
	                 DPRC_CMDSZ_GET_DEV_COUNT, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_DEV_COUNT(RSP_READ);
	}

	return err;
}

int dprc_get_device(struct dprc *dprc,
                    int dev_index,
                    struct dprc_dev_desc *dev_desc)
{
	struct mc_cmd_data cmd_data = { 0 };
	int err;
	uint16_t type_def;
	DPRC_CMD_GET_DEVICE(CMD_PREP);

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_DEVICE,
	                 DPRC_CMDSZ_GET_DEVICE, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_DEVICE(RSP_READ_STRUCT);
		dev_desc->type = type_def;
	}

	return err;
}

int dprc_get_res_count(struct dprc *dprc,
                       uint16_t res_type,
                       int *res_count)
{
	struct mc_cmd_data cmd_data = { 0 };
	uint16_t type_def = res_type;
	int err;

	DPRC_CMD_GET_RES_COUNT(CMD_PREP);

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_RES_COUNT,
	                 DPRC_CMDSZ_GET_RES_COUNT, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_RES_COUNT(RSP_READ);
	}

	return err;
}

int dprc_get_res_ids(struct dprc *dprc,
                     uint16_t res_type,
                     struct dprc_res_ids_range_desc *range_desc) 
{

	struct mc_cmd_data cmd_data = { 0 };
	uint16_t res_type_def = res_type;
	int err;

	DPRC_CMD_GET_RES_IDS(CMD_PREP);

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_RES_IDS,
	                 DPRC_CMDSZ_GET_RES_IDS, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_RES_IDS(RSP_READ_STRUCT); 
	}

	return err;
}

int dprc_get_attributes(struct dprc *dprc, struct dprc_attributes *attr)
{
	struct mc_cmd_data cmd_data = { 0 };
	int err;

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_ATTR,
	                 DPRC_CMDSZ_GET_ATTR, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_ATTRIBUTES(RSP_READ_STRUCT);
	}

	return err;
}

int dprc_get_dev_region(struct dprc *dprc,
                        uint16_t dev_type,
                        uint16_t dev_id,
                        uint8_t region_index,
                        struct dprc_region_desc *region_desc)
{
	struct mc_cmd_data cmd_data = { 0 };
	int err;
	uint16_t dev_type_def = dev_type;
	/* prepare command */
	DPRC_CMD_GET_DEV_REGION(CMD_PREP);

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_DEV_REG,
	                 DPRC_CMDSZ_GET_DEV_REG, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_DEV_REGION(RSP_READ_STRUCT);
	}

	return err;
}

int dprc_set_irq(struct dprc *dprc,
                 uint8_t irq_index,
                 uint64_t irq_paddr,
                 uint32_t irq_val)
{
	struct mc_cmd_data cmd_data = { 0 };

	DPRC_CMD_SET_IRQ(CMD_PREP);

	return cmdif_send(&(dprc->cidesc), DPRC_CMDID_SET_IRQ,
	                  DPRC_CMDSZ_SET_IRQ, CMDIF_PRI_LOW,
	                  (uint8_t*)&cmd_data);
}

int dprc_get_irq(struct dprc *dprc,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val)
{
	struct mc_cmd_data cmd_data = { 0 };
	int err;

	DPRC_CMD_GET_IRQ(CMD_PREP);

	err = cmdif_send(&(dprc->cidesc), DPRC_CMDID_GET_IRQ,
	                 DPRC_CMDSZ_GET_IRQ, CMDIF_PRI_LOW,
	                 (uint8_t*)&cmd_data);
	if (!err) {
		/* retrieve response parameters */
		DPRC_RSP_GET_IRQ(RSP_READ);
	}

	return err;
}
