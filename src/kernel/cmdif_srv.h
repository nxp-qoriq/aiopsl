#ifndef __CMDIF_SRV_H
#define __CMDIF_SRV_H

#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "fsl_cmdif_flib.h"

#define CMD_ID_MASK	   0x00000000FFFF0000 /**< FLC */
#define CMD_ID_OFF	   16

#define AUTH_ID_MASK	   0x0000FFFF00000000 /**< FLC[hash] */
#define AUTH_ID_OFF	   32
#define ERROR_MASK	   0x00FF000000000000 /**< FLC[hash] */
#define ERROR_OFF	   48
#define DEV_H_MASK	   0xFF00000000000000 /**< FLC[hash] */
#define DEV_H_OFF	   56
#define INST_ID_MASK	   DEV_H_MASK         /**< FLC[hash] */
#define INST_ID_OFF	   DEV_H_OFF

#define CMD_ID_OPEN           0x8000
#define CMD_ID_CLOSE          0x4000
#define M_NUM_OF_INSTANCES    1000
#define M_NUM_OF_MODULES      64
#define M_NAME_CHARS          8     /**< Not including \0 */
#define SYNC_BUFF_RESERVED    1     /**< 1 Byte must be reserved for done bit */

#define OPEN_AUTH_ID          0xFFFF
/**< auth_id that will be sent as hash value for open commands */

struct cmdif_srv {
	char         (*m_name)[M_NAME_CHARS + 1];
	/**< pointer to arrays of module name per module, DDR */
	open_cb_t    **open_cb;
	/**< open(init) callbacks, one per module, DDR */
	close_cb_t   **close_cb;
	/**< close(de-init) callbacks, one per module, DDR*/
	ctrl_cb_t    **ctrl_cb;
	/**< execution callbacks one per module, SHRAM */
	fsl_handle_t *inst_dev;
	/**< array of instances handels(converted from the authentication ID)
	 * in the size of M_NUM_OF_INSTANCES, SHRAM */
	uint64_t     *sync_done;
	/**< array of physical addresses per instance for setting done
	 * for synchronious commands, SHRAM */
	uint8_t      *m_id;
	/**< converts auth_id to module for cb, SHRAM */
	uint16_t     inst_count;
	/**< counter for instance handlers */
	uint8_t      lock;
	/**< cmdif spinlock used for module id allocation */
};

int cmdif_srv_init(void);
void cmdif_srv_free(void);
void cmdif_srv_isr(void);

#endif /* __CMDIF_SRV_H */
