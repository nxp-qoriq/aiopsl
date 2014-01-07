#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_malloc.h"
#include "common/fsl_cmdif.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dprc_cmd.h"
#include "arch/fsl_cmdif_mc.h"
#include "dprc.h"

static void prepare_create_container_cmd(struct cmdif_cmd_data *desc,
                                         struct dprc_create_attributes *attr)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param,
	                DPRC_CREATE_CONT_ICID_O,
	                DPRC_CREATE_CONT_ICID_S,
	                attr->icid);
	cmd_param = u64_write_field(cmd_param,
	                DPRC_CREATE_CONT_SPAWN_O,
	                DPRC_CREATE_CONT_SPAWN_S,
	                attr->spawn_policy);
	cmd_param = u64_write_field(cmd_param,
	                DPRC_CREATE_CONT_ALLOC_O,
	                DPRC_CREATE_CONT_ALLOC_S,
	                attr->alloc_policy);
	cmd_param = u64_write_field(cmd_param,
	                DPRC_CREATE_CONT_OPTIONS_O,
	                DPRC_CREATE_CONT_OPTIONS_S,
	                attr->options);
	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_destroy_container_cmd(struct cmdif_cmd_data *desc,
                                          int container_id)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param,
	                DPRC_DESTROY_CONT_ID_O,
	                DPRC_DESTROY_CONT_ID_S, container_id);

	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_set_res_policy_cmd(struct cmdif_cmd_data *desc,
                                       int container_id,
                                       enum dp_res_type type,
                                       enum dprc_alloc_policy alloc_policy,
                                       uint16_t quota)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_SET_RES_ALLOC_P_CONT_ID_O,
	                DPRC_SET_RES_ALLOC_P_CONT_ID_S, container_id);
	cmd_param = u64_write_field(cmd_param, DPRC_SET_RES_ALLOC_P_RES_TYPE_O,
	                DPRC_SET_RES_ALLOC_P_RES_TYPE_S, type);
	cmd_param = u64_write_field(cmd_param,
	                DPRC_SET_RES_ALLOC_P_O,
	                DPRC_SET_RES_ALLOC_P_S,
	                alloc_policy);
	cmd_param = u64_write_field(cmd_param,
	                DPRC_SET_RES_ALLOC_P_QUOTA_O,
	                DPRC_SET_RES_ALLOC_P_QUOTA_S,
	                quota);
	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_get_res_policy_cmd(struct cmdif_cmd_data *desc,
                                       int container_id,
                                       enum dp_res_type type)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_GET_RES_ALLOC_P_CONT_ID_O,
	                DPRC_GET_RES_ALLOC_P_CONT_ID_S, container_id);
	cmd_param = u64_write_field(cmd_param, DPRC_GET_RES_ALLOC_P_RES_TYPE_O,
	                DPRC_GET_RES_ALLOC_P_RES_TYPE_S, type);
	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_reset_container_cmd(struct cmdif_cmd_data *desc,
                                        int container_id)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_RST_CONT_ID_O, DPRC_RST_CONT_ID_S,
	                container_id);

	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_assign_cmd(struct cmdif_cmd_data *desc,
                               int container_id,
                               struct dprc_res_req *res_req)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_ASSIGN_CONT_ID_O, DPRC_ASSIGN_CONT_ID_S,
	                container_id);
	cmd_param = u64_write_field(cmd_param, DPRC_ASSIGN_RES_TYPE_O,
	                DPRC_ASSIGN_RES_TYPE_S, res_req->type);
	cmd_param = u64_write_field(cmd_param, DPRC_ASSIGN_ALIGN_O, DPRC_ASSIGN_ALIGN_S,
	                res_req->base_align);
	cmd_param = u64_write_field(cmd_param, DPRC_ASSIGN_NUM_OF_RES_O,
	                DPRC_ASSIGN_NUM_OF_RES_S, res_req->num);

	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
	/* build param 2 */
	cmd_param = 0;
	cmd_param = u64_write_field(cmd_param, DPRC_ASSIGN_OPTIONS_O, DPRC_ASSIGN_OPTIONS_S,
	                res_req->options);

	GPP_CMD_WRITE_PARAM(desc, 2, cmd_param);
}

static void prepare_unassign_cmd(struct cmdif_cmd_data *desc,
                                 int container_id,
                                 struct dprc_res_req *res_req)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_UNASSIGN_CONT_ID_O,
	                DPRC_UNASSIGN_CONT_ID_S, container_id);
	cmd_param = u64_write_field(cmd_param, DPRC_UNASSIGN_RES_TYPE_O,
	                DPRC_UNASSIGN_RES_TYPE_S, res_req->type);
	cmd_param = u64_write_field(cmd_param, DPRC_UNASSIGN_ALIGN_O, DPRC_UNASSIGN_ALIGN_S,
	                res_req->base_align);
	cmd_param = u64_write_field(cmd_param, DPRC_UNASSIGN_NUM_OF_RES_O,
	                DPRC_UNASSIGN_NUM_OF_RES_S, res_req->num);

	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
	/* build param 2 */
	cmd_param = 0;
	cmd_param = u64_write_field(cmd_param, DPRC_UNASSIGN_OPTIONS_O,
	                DPRC_UNASSIGN_OPTIONS_S, res_req->options);

	GPP_CMD_WRITE_PARAM(desc, 2, cmd_param);
}

static void prepare_get_device_cmd(struct cmdif_cmd_data *desc,
                                    int dev_index)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_GET_DEV_IDX_O,
	                DPRC_GET_DEV_IDX_S, dev_index);
	GPP_CMD_WRITE_PARAM(desc, 3, cmd_param);
}

static void receive_get_device_cmd(struct cmdif_cmd_data *desc,
                                   struct dprc_dev_desc *dev_desc)
{
	uint64_t cmd_param = 0;
	cmd_param = CMDIF_MC_READ_PARAM(desc, 1 );

	dev_desc->rev_minor = (uint8_t)u64_read_field(cmd_param,
	                                          DPRC_GET_DEV_REV_MINOR_O,
	                                          DPRC_GET_DEV_REV_MINOR_S);
	dev_desc->rev_major = (uint8_t)u64_read_field(cmd_param,
	                                              DPRC_GET_DEV_REV_MAJOR_O,
	                                              DPRC_GET_DEV_REV_MAJOR_S);
	dev_desc->type = (uint16_t)u64_read_field(cmd_param,
	                                          DPRC_GET_DEV_TYPE_O,
	                                          DPRC_GET_DEV_TYPE_S);
	dev_desc->id = (uint32_t)u64_read_field(cmd_param, DPRC_GET_DEV_ID_O,
	                                        DPRC_GET_DEV_ID_S);
	/* read param 2 */
	cmd_param = CMDIF_MC_READ_PARAM(desc, 2 );

	dev_desc->vendor = (uint16_t)u64_read_field(cmd_param,
	                                            DPRC_GET_DEV_VENDOR_O,
	                                            DPRC_GET_DEV_VENDOR_S);
	cmd_param = CMDIF_MC_READ_PARAM(desc, 3 );
	dev_desc->irq_count = (uint8_t)u64_read_field(cmd_param,
	                                               DPRC_GET_DEV_IRQS_O,
	                                               DPRC_GET_DEV_IRQS_S);
	dev_desc->region_count = (uint8_t)u64_read_field(cmd_param,
	                                               DPRC_GET_DEV_REGIONS_O,
	                                               DPRC_GET_DEV_REGIONS_S);
	dev_desc->state = (uint32_t)u64_read_field(cmd_param,
	                                           DPRC_GET_DEV_STATE_O,
	                                           DPRC_GET_DEV_STATE_S);
}

static void prepare_get_res_count_cmd(struct cmdif_cmd_data *desc,
                                      enum dp_res_type type)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_GET_RES_COUNT_TYPE_O,
	                DPRC_GET_RES_COUNT_TYPE_S, type);
	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_get_res_ids_cmd(struct cmdif_cmd_data *desc,
                                    enum dp_res_type res_type,
                                    int num_of_ids)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_GET_RES_IDS_RES_TYPE_O,
	                DPRC_GET_RES_IDS_RES_TYPE_S, res_type);
	cmd_param = u64_write_field(cmd_param, DPRC_GET_RES_IDS_NUM_OF_IDS_O,
	                DPRC_GET_RES_IDS_NUM_OF_IDS_S, num_of_ids);

	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_get_dev_region(struct cmdif_cmd_data *desc,
                                   uint16_t dev_type,
                                   uint16_t dev_id,
                                   uint8_t region_index)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_GET_DEV_REG_DEV_ID_O,
	                DPRC_GET_DEV_REG_DEV_ID_S, dev_id);
	cmd_param = u64_write_field(cmd_param, DPRC_GET_DEV_REG_DEV_TYPE_O,
	                DPRC_GET_DEV_REG_DEV_TYPE_S, dev_type);
	cmd_param = u64_write_field(cmd_param, DPRC_GET_DEV_REG_IDX_O,
	                DPRC_GET_DEV_REG_IDX_S, region_index);

	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
}

static void prepare_set_irq(struct cmdif_cmd_data *desc,
                            uint8_t irq_index,
                            uint64_t irq_paddr,
                            uint32_t irq_val)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_SET_IRQ_VAL_O,
	                DPRC_SET_IRQ_VAL_S, irq_val);
	cmd_param = u64_write_field(cmd_param, DPRC_SET_IRQ_IDX_O,
	                DPRC_SET_IRQ_IDX_S, irq_index);
	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);
	cmd_param = 0;
	cmd_param = u64_write_field(cmd_param, DPRC_SET_IRQ_PADDR_O,
	                DPRC_SET_IRQ_PADDR_S, irq_paddr);

	GPP_CMD_WRITE_PARAM(desc, 2, cmd_param);
}

static void prepare_get_irq(struct cmdif_cmd_data *desc,
                            uint8_t irq_index)
{
	uint64_t cmd_param = 0;
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param, DPRC_GET_IRQ_IDX_O,
	                DPRC_GET_IRQ_IDX_S, irq_index);
	GPP_CMD_WRITE_PARAM(desc, 1, cmd_param);

}

static void receive_get_dev_count_cmd(struct cmdif_cmd_data *desc, int *count)
{
	uint64_t cmd_param = GPP_CMD_READ_PARAM(desc, 1);
	*count = (int)u64_read_field(cmd_param, DPRC_GET_DEV_COUNT_COUNT_O,
	                             DPRC_GET_DEV_COUNT_COUNT_S);
}

static void receive_get_res_count_cmd(struct cmdif_cmd_data *desc, int *count)
{
	uint64_t cmd_param = GPP_CMD_READ_PARAM(desc, 1);
	*count = (int)u64_read_field(cmd_param, DPRC_GET_RES_COUNT_COUNT_O,
	                             DPRC_GET_RES_COUNT_COUNT_S);
}
int dprc_get_container_id(struct dprc *dprc, int *container_id)
{
	/*TODO - review*/
	struct cmdif_cmd_data *cmd_data;
	int err;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	uint64_t cmd_param = 0;
	/* prepare command */
	/* build param 1*/
	cmd_param = u64_write_field(cmd_param,
	                DPRC_GET_CONT_ID_PORTAL_O,
	                DPRC_GET_CONT_ID_PORTAL_S,
	                (int)dprc->cidesc.regs);/*TODO - check*/
	GPP_CMD_WRITE_PARAM(cmd_data, 1, cmd_param);

	/* send command to mc*/

	err = cmdif_send(&(dprc->cidesc), DPRC_CMD_GET_CONT_ID, DPRC_CMD_GET_CONT_ID_S,
	                 CMDIF_PRI_LOW, cmd_data);
	/* recieve out parameters */
	if (!err)
	{
		cmd_param = GPP_CMD_READ_PARAM(cmd_data, 1);
		*container_id = (int)u64_read_field(cmd_param,
		                                    DPRC_GET_CONT_ID_O,
		                                    DPRC_GET_CONT_ID_S);
	}
	return err;
}

int dprc_open(struct dprc *dprc, int container_id)
{
	return cmdif_open(&(dprc->cidesc), FSL_OS_MOD_DPSP, (uint16_t)container_id);
}

int dprc_close(struct dprc *dprc)
{
	return cmdif_close(&(dprc->cidesc));
}

int dprc_create_container(struct dprc 			*dprc,
                          struct dprc_create_attributes *attributes,
                          int 				*child_container_id,
                          uint64_t 			*child_portal_paddr)
{
	struct cmdif_cmd_data *cmd_data;
	int err;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	uint64_t cmd_param;
	/* write command body */

	prepare_create_container_cmd(cmd_data, attributes);

	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc), DPRC_CMD_CREATE_CONT, DPRC_CMD_CREATE_CONT_S,
	                 CMDIF_PRI_LOW, cmd_data);
	/* recieve out parameters */
	if (!err) {
		cmd_param = GPP_CMD_READ_PARAM(cmd_data, 2);
		*child_container_id = (int)u64_read_field(cmd_param,
		                                  DPRC_CREATE_CONT_CHILD_ID_O,
		                                  DPRC_CREATE_CONT_CHILD_ID_S);
		*child_portal_paddr = (uint32_t)u64_read_field(cmd_param,
		                           DPRC_CREATE_CONT_CHILD_PORTAL_O,
		                           DPRC_CREATE_CONT_CHILD_PORTAL_S);
	}
	return err;
}

int dprc_destroy_container(struct dprc *dprc, int child_container_id)
{
	
	struct cmdif_cmd_data *cmd_data;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);

	/* write command body */
	prepare_destroy_container_cmd(cmd_data, child_container_id);
	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc),
	                  DPRC_CMD_DESTROY_CONT,
	                  DPRC_CMD_DESTROY_CONT_S,
	                  CMDIF_PRI_LOW,
	                  cmd_data);
}

int dprc_set_res_alloc_policy(struct dprc *dprc,
                              int child_container_id,
                              enum dp_res_type res_type,
                              enum dprc_alloc_policy alloc_policy,
                              uint16_t quota)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	err = cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);

	/* write command body */
	prepare_set_res_policy_cmd(cmd_data,
	                           child_container_id,
	                           res_type,
	                           alloc_policy,
	                           quota);

	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc), DPRC_CMD_SET_RES_ALLOC_P,
	                  DPRC_CMD_SET_RES_ALLOC_P_S, CMDIF_PRI_LOW, cmd_data);
}

int dprc_get_res_alloc_policy(struct dprc *dprc,
                              int child_container_id,
                              enum dp_res_type res_type,
                              enum dprc_alloc_policy *alloc_policy,
                              uint16_t *quota)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	uint64_t cmd_param;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);

	/* write command body */
	prepare_get_res_policy_cmd(cmd_data,
	                           child_container_id,
	                           res_type);

	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc), DPRC_CMD_SET_RES_ALLOC_P,
	                  DPRC_CMD_SET_RES_ALLOC_P_S, CMDIF_PRI_LOW, cmd_data);
	/* recieve out parameters */
	if (!err)
	{
		cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
		if (!err) {
			cmd_param = GPP_CMD_READ_PARAM(cmd_data, 1);
			*alloc_policy = (enum dprc_alloc_policy)u64_read_field(
							cmd_param,
							DPRC_GET_RES_ALLOC_P_O,
							DPRC_GET_RES_ALLOC_P_S);
			*quota = (uint16_t)u64_read_field(cmd_param,
							  DPRC_GET_RES_ALLOC_P_QUOTA_O,
							  DPRC_GET_RES_ALLOC_P_QUOTA_S);
		}
	}
	return err;
}

int dprc_reset_container(struct dprc *dprc, int child_container_id)
{
	
	struct cmdif_cmd_data *cmd_data;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	/* write command body */
	prepare_reset_container_cmd(cmd_data, child_container_id);

	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc), DPRC_CMD_RST_CONT, DPRC_CMD_RST_CONT_S,
	                  CMDIF_PRI_LOW, cmd_data);
}

int dprc_assign(struct dprc *dprc,
                int container_id,
                struct dprc_res_req *res_req)
{
	
	struct cmdif_cmd_data *cmd_data;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);

	/* write command body */
	prepare_assign_cmd(cmd_data, container_id, res_req);

	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc), DPRC_CMD_ASSIGN, DPRC_CMD_ASSIGN_S,
	                  CMDIF_PRI_LOW, cmd_data);
}

int dprc_unassign(struct dprc *dprc,
                  int child_container_id,
                  struct dprc_res_req *res_req)
{
	
	struct cmdif_cmd_data *cmd_data;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);

	/* write command body */
	prepare_unassign_cmd(cmd_data, child_container_id, res_req);

	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc), DPRC_CMD_UNASSIGN, DPRC_CMD_UNASSIGN_S,
	                  CMDIF_PRI_LOW, cmd_data);
}

int dprc_get_device_count(struct dprc *dprc, int *dev_count)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc), DPRC_CMD_GET_DEV_COUNT, DPRC_CMD_GET_DEV_COUNT_S,
	                 CMDIF_PRI_LOW, cmd_data);
	if (!err)
		receive_get_dev_count_cmd(cmd_data, dev_count);
	return err;
}

int dprc_get_device(struct dprc *dprc,
                    int dev_index,
                    struct dprc_dev_desc *dev_desc)
{
	
	struct cmdif_cmd_data *cmd_data;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	/* write command body */
	prepare_get_device_cmd(cmd_data, dev_index);

	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc), DPRC_CMD_GET_DEVICE,
		                 DPRC_CMD_GET_DEVICE_S, CMDIF_PRI_LOW, cmd_data);
	if (!err) {
		cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
		if (!err)
			receive_get_device_cmd(cmd_data, dev_desc);
	}
	return err;
}

int dprc_get_res_count(struct dprc *dprc, enum dp_res_type res_type, int *res_count)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	prepare_get_res_count_cmd(cmd_data, res_type);
	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc), DPRC_CMD_GET_RES_COUNT, DPRC_CMD_GET_RES_COUNT_S,
	                 CMDIF_PRI_LOW, cmd_data);
	if (!err)
		receive_get_res_count_cmd(cmd_data, res_count);
	return err;
}

int dprc_get_res_ids(struct dprc *dprc,
                               enum dp_res_type res_type,
                               int res_ids_num,
                               uint32_t *res_ids,
                               int *valid_count) /*TODO - add valid count */
{
	
	struct cmdif_cmd_data *cmd_data;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);

	/* write command body */
	prepare_get_res_ids_cmd(cmd_data, res_type, res_ids_num);

	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc), DPRC_CMD_GET_RES_IDS, DPRC_CMD_GET_RES_IDS_S,
	                  CMDIF_PRI_LOW, cmd_data);
}

int dprc_get_attributes(struct dprc *dprc, struct dprc_attributes *attributes)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	uint64_t cmd_param;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc),
	                 DPRC_CMD_GET_ATTR,
	                 DPRC_CMD_GET_ATTR_S,
	                 CMDIF_PRI_LOW,
	                 cmd_data);
	/* recieve out parameters */
	if (!err)
	{
		cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
		if (!err) {
			cmd_param = GPP_CMD_READ_PARAM(cmd_data, 1);
			attributes->container_id = (int)u64_read_field(
							cmd_param,
							DPRC_GET_ATTR_CONT_ID_O,
							DPRC_GET_ATTR_CONT_ID_S);
			attributes->icid = (uint16_t)u64_read_field(
							cmd_param,
							DPRC_GET_ATTR_ICID_O,
							DPRC_GET_ATTR_ICID_S);
			attributes->portal_id = (uint16_t)u64_read_field(
							cmd_param,
							DPRC_GET_ATTR_PORTAL_O,
							DPRC_GET_ATTR_PORTAL_S);
	
			cmd_param = GPP_CMD_READ_PARAM(cmd_data, 2);
			attributes->options = (uint32_t)u64_read_field(
							cmd_param,
							DPRC_GET_ATTR_OPTIONS_O,
							DPRC_GET_ATTR_OPTIONS_S);
			attributes->spawn_policy = (enum dprc_spawn_policy)
					u64_read_field(cmd_param,
							DPRC_GET_ATTR_SPAWN_O,
							DPRC_GET_ATTR_SPAWN_S);
			attributes->allocation_policy = (enum dprc_alloc_policy)
					u64_read_field(cmd_param,
							DPRC_GET_ATTR_ALLOC_O,
							DPRC_GET_ATTR_ALLOC_S);
		}
	}
	return err;
}

int dprc_get_dev_region(struct dprc *dprc,
                        uint16_t dev_type,
                        uint16_t dev_id,
                        uint8_t region_index,
                        struct dprc_region_desc *region_desc)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	uint64_t cmd_param;
	/* prepare command */
	prepare_get_dev_region(cmd_data, dev_type, dev_id, region_index);
	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc),
	                 DPRC_CMD_GET_DEV_REG,
	                 DPRC_CMD_GET_DEV_REG_S,
	                 CMDIF_PRI_LOW,
	                 cmd_data);
	/* recieve out parameters */
	if (!err)
	{
		cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
		if (!err) {
			cmd_param = GPP_CMD_READ_PARAM(cmd_data, 1);
			region_desc->size= (uint16_t)u64_read_field(cmd_param,
							       DPRC_GET_DEV_REG_SIZE_O,
							       DPRC_GET_DEV_REG_SIZE_S);
			cmd_param = GPP_CMD_READ_PARAM(cmd_data, 2);
			region_desc->base_paddr= (uint64_t)u64_read_field(cmd_param,
							 DPRC_GET_DEV_REG_BASE_PADDR_O,
							 DPRC_GET_DEV_REG_BASE_PADDR_S);
		}
	}
	return err;
}

int dprc_set_irq(struct dprc *dprc,
                 uint8_t irq_index,
                 uint64_t irq_paddr,
                 uint32_t irq_val)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	err = cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	/* prepare command */
	prepare_set_irq(cmd_data, irq_index, irq_paddr, irq_val);
	/* send command to mc*/
	return cmdif_send(&(dprc->cidesc),
	                  DPRC_CMD_SET_IRQ,
	                  DPRC_CMD_SET_IRQ_S,
	                  CMDIF_PRI_LOW,
	                  cmd_data);
}

int dprc_get_irq(struct dprc *dprc,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val)
{
	
	struct cmdif_cmd_data *cmd_data;
	int err;
	cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
	uint64_t cmd_param;
	/* prepare command */
	prepare_get_irq(cmd_data, irq_index);
	/* send command to mc*/
	err = cmdif_send(&(dprc->cidesc),
	                  DPRC_CMD_GET_IRQ,
	                  DPRC_CMD_GET_IRQ_S,
	                  CMDIF_PRI_LOW,
	                  cmd_data);
	if (!err)
	{
		cmdif_get_cmd_data(&(dprc->cidesc), &cmd_data);
		if (!err) {
			cmd_param = GPP_CMD_READ_PARAM(cmd_data, 1);
			*irq_val= (uint32_t)u64_read_field(cmd_param,
							   DPRC_GET_IRQ_VAL_O,
							   DPRC_GET_IRQ_VAL_S);
			cmd_param = GPP_CMD_READ_PARAM(cmd_data, 2);
			*irq_paddr= (uint64_t)u64_read_field(cmd_param,
							     DPRC_GET_IRQ_PADDR_O,
							     DPRC_GET_IRQ_PADDR_S);
		}
	}
	return err;
}
