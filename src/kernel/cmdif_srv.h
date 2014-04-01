#ifndef __CMDIF_SRV_H
#define __CMDIF_SRV_H

#include "common/fsl_aiop_cmdif.h"

#define CMD_ID_MASK	   0x00000000FFFF0000
#define CMD_ID_OFF	   16
#define CMD_ID_OPEN        0x8000
#define CMD_ID_CLOSE       0x4000
#define CMD_AUTH_ID_MASK   0x0000FFFF

#define M_NUM_OF_INSTANCES    1000
#define M_NUM_OF_MODULES      64
#define M_NAME_CHARS          8     /**< Not including \0 */

struct cmdif_srv {
	char         (*m_name)[M_NAME_CHARS + 1];
	/**< pointer to arrays of module name per module */
	ctrl_cb_t    **ctrl_cb;
	/**< execution callbacks one per module */
	fsl_handle_t *instance_handle;
	/**< array of instances handels(converted from the authentication ID)
	 * in the size of MAX_NUM_OF_INSTANCES */
	uint8_t      *m_id;
	/**< converts ID to module for cb */
	uint16_t     instances_counter;
	/*DDR structures */
	open_cb_t    **open_cb;
	/**< open(init) callbacks, one per module*/
	close_cb_t   **close_cb;
	/**< close(de-init) callbacks, one per module*/
	uint8_t      lock;
	/**< cmdif spinlock used for module id allocation */
};

int cmdif_srv_init(void);
void cmdif_srv_free(void);
void cmdif_srv_isr(void);

#endif /* __CMDIF_SRV_H */
