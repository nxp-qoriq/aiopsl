#ifndef __CMDIF_SRV_H
#define __CMDIF_SRV_H

#include "common/fsl_aiop_cmdif.h"

#define CMD_ID_MASK	0x00000000FFFF0000
#define CMD_ID_OFF	16
#define CMD_ID_OPEN     0x8000

struct cmdif_srv {
	ctrl_cb_t **ctrl_cb;
	/**< execution callbacks one per module */
	fsl_handle_t *instance_handle;
	/**< array of instances handels(converted from the authentication ID)
	 * in the size of MAX_NUM_OF_INSTANCES */
	uint8_t *module_id;
	/**< converts ID to module for cb */
	uint16_t instances_counter;
	/*DDR structures */
	open_cb_t **open_cb;
	/**< open(init) callbacks, one per module*/
	close_cb_t **close_cb;
	/**< close(de-init) callbacks, one per module*/
};

int cmdif_srv_init(void);
void cmdif_srv_free(void);
void cmdif_srv_isr(void);

#endif /* __CMDIF_SRV_H */
