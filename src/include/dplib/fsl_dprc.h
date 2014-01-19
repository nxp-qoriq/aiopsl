/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dprc.h
 *  @brief   Data Path Resource Container API
 */

#ifndef _FSL_DPRC_H
#define _FSL_DPRC_H

#include <fsl_cmdif.h>


#ifdef MC
struct dprc;
#else
struct dprc {
	struct cmdif_desc cidesc;	/*!< Descriptor for command portal */
};
#endif
/*!
 * @Group grp_dprc	Data Path Resource Container API
 *
 * @brief	Contains DPRC API for managing and querying LDPAA resources
 * @{
 */

/*!
 * Set this value as the ICID value in dprc_cfg structure when creating a
 * container, in case the ICID is not selected by the user and should be
 * allocated by the DPRC from the pool of ICIDs.
 */
#define DPRC_GET_ICID_FROM_POOL		(uint16_t)(~(0))

/**
 * Resource types defines
 */
#define	DP_RES_ICID  0
	/*!< Isolation contexts ID */
#define	DP_RES_MCPID  1
	/*!< Management portals */
#define	DP_RES_SWP  2
	/*!< Software portals */
#define	DP_RES_BPID  3
	/*!< Buffer pools */
#define	DP_RES_WQ_CHANNEL  4
	/*!< Channels */
#define	DP_RES_FQID  5
	/*!< Frame queues */
#define	DP_RES_QPR  6
	/*!< Queuing priority record */
#define	DP_RES_QDID  7
	/*!< Queuing destinations */
#define	DP_RES_CGID  8
	/*!< Congestion groups */
#define	DP_RES_CEETM_CQ_CHANNEL  11
	/*!< Class Queue Channel ID */
#define	DP_RES_TABLE_ID  13
	/*!< Classification tables */
#define	DP_RES_KEY_PROFILE_ID  14
	/*!< Key ID*/
#define	DP_RES_PLPID  15
	/*!< Policer profiles ID */
#define	DP_RES_PRPID  16
	/*!< Parser profile ID */
#define	DP_RES_PPID  17
	/*!< Physical ports */
#define	DP_RES_IFPID  18
	/*!< Interface profiles */
#define	DP_RES_TASK  19
	/*!< AIOP tasks*/
#define	DP_RES_RPLR  22
	/*!< Replication list record */
#define	DP_RES_DPSW_PORT  23
	/*!< DPSW port*/
#define DP_RES_POLICY_ID  24
	/*!< Policy ID */


/**
 * Device types definition
 */
#define	DP_DEV_DPRC 100
	/*!< DPRC Device */
#define	DP_DEV_DPNI 101
	/*!< DPNI Device*/
#define	DP_DEV_DPIO 102
	/*!< DPIO device */
#define	DP_DEV_DPSP 103
	/*!< DPSP device */
#define	DP_DEV_DPSW 104
	/*!< DPSW device */
#define	DP_DEV_DPDMUX 105
	/*!< DPMAC device */
#define	DP_DEV_DPMAC 106
	/*!< DPMAC device */
/* More will be added... */


/*!
 * @name Resource request options
 */
#define DPRC_RES_REQ_OPT_EXPLICIT		0x00000001
/*!< Explicit resource id request - Relevant only for resources
 * request. The requested resources are explicit and sequential The base ID is
 * given at res_req at base_align field */
#define DPRC_RES_REQ_OPT_ALIGNED		0x00000002
/*!< Sequential resources request - Relevant only for resources
 * request. Indicates that resources id should be sequential and aligned to the
 * value given at dprc_res_req base_align field */
#define DPRC_RES_REQ_OPT_PLUGGED		0x00000004
/*!< Plugged Flag - Relevant only for device assignment request.
 * Indicates that after all devices assigned. An interrupt will be invoked at
 * the relevant GPP. The assigned device will be marked as plugged */
#define DPRC_RES_REQ_OPT_SHARED			0x00000008
/*!< Shared flag - Relevant only for device assignment request.
 * In case this flag is set the device will be copied to the containers on
 * assignment otherwise it will be moved from its container to the new one */
/* @} */

/*!
 * @name Container general options
 *
 * These options may be selected at container creation by the container creator
 * and can be retreived using dprc_get_attributes()
 */
#define DPRC_CFG_OPT_SPAWN_ALLOWED           	0x00000001
/*!< Spawn Policy Option allowed - Indicates that the new container is alllowd
 * to span and have its own child containers. */
#define DPRC_CFG_OPT_ALLOC_ALLOWED           	0x00000002
/*!< General Container allocation policy - Indicates that the new container is
 * allowed to allocate requested resources from its parent container; if not
 * set, the container is only allowed to use resources in its own pools; Note
 * that this is a container's global policy, but the parent container may
 * override it and set specific quota per resource type. */
#define DPRC_CFG_OPT_DEVICE_INIT_ALLOWED        0x00000004
/*!< Device initialization allowed - software context associated with this
 * container is allowed to invoke device intiialization operations. */
#define DPRC_CFG_OPT_TOPOLOGY_CHANGES_ALLOWED	0x00000008
/*!< Topology change allowed - software context associated with this
 * container is allowed to invoke topology operations, such as attach/detach
 * of network devices. */
#define DPRC_CFG_OPT_IOMMU_BYPASS		0x00000010
/*!<IOMMU bypass - indicates whether devices of this container are permitted
 * to bypass the IOMMU.  */
/* @} */

/*!
 * @name Device Attributes Flags
 */
#define DPRC_DEV_STATE_OPEN		0x00000001
/*!< Opened state - Indicates that a device was opened by at least one owner */
#define DPRC_DEV_STATE_PLUGGED		0x00000002
/*!< Plugged state - Indicates that a device is plugged */
#define DPRC_DEV_STATE_SHARED		0x00000004
/*!< Shared state - Indicates that a device is shared by more than one container */
/* @} */


/**
 * @brief	Resource request descriptor, to be used in assignment or
 * 		un-assignment of resources and devices.
 */
struct dprc_res_req {
	uint16_t type;
	/*!< Resource/device type: one of DP_RES_ or DP_DEV_ values;
	 * Note: it is not possible to assign/unassign DP_DEV_DPRC devices */
	uint32_t num;
	/*!< Number of resources */
	uint32_t options;
	/*!< Request options: combination of DPRC_RES_REQ_OPT_ options */
	int id_base_align;
	/*!<
	 * In case of explicit assignment, this field represents the
	 * required base ID for resource allocation;
	 * In case of non-explicit
	 * assignment, this field indicates the required alignment for the
	 * resource ID(s) - use 0 or 1 if there is no alignment requirement.
	 */
};

/**
 * @brief	Device descriptor, returned from dprc_get_device()
 */
struct dprc_dev_desc {
	uint16_t vendor;
	/*!< Device vendor identifier */
	uint16_t type;
	/*!< Type of device: one of DP_DEV_ values */
	int id;
	/*!< ID of logical device resource */
	uint8_t rev_major;
	/*!< Major revision number */
	uint8_t rev_minor;
	/*!< Minor  revision number */
	uint8_t irq_count;
	/*!< Number of interrupts supported by the device */
	uint8_t region_count;
	/*!< Number of mappable regions supported by the device */
	uint32_t state;
	/*!< Device state: combination of DPRC_DEV_STATE_ states */
};

/**
 * @brief	Mappable region descriptor, returned by dprc_get_dev_region()
 */
struct dprc_region_desc {
	uint64_t base_paddr;
	/*!< Region base physical address */
	uint16_t size;
	/*!< Region size (in bytes) */
};

/**
 * @brief	Iteration status, for use with dprc_get_res_ids() function
 *
 */
enum dprc_iter_status {
	DPRC_ITER_STATUS_FIRST = 0,
	/*!< Perform first iteration */
	DPRC_ITER_STATUS_MORE = 1,
	/*!< Indicates more/next iteration is needed */
	DPRC_ITER_STATUS_LAST = 2,
	/*!< Indicates last iteration */
};

/**
 * @brief	Resource Id rangen descriptor, Used at dprc_get_res_ids() and
 * 		contains one range details.
 */
struct dprc_res_ids_range_desc {
	int base_id;
	/*!< Base resource ID of this range */
	int last_id;
	/*!< Last resource ID of this range */
	enum dprc_iter_status iter_status;
	/*!< Iterartion status - should be set to DPRC_ITER_STATUS_FIRST at
	 * first iteration; while the returned marker is DPRC_ITER_STATUS_MORE,
	 * additional iterations are needed, until the returned marker is
	 * DPRC_ITER_STATUS_LAST. */
};

/**
 * @brief	Container attributes, returned by dprc_get_attributes()
 */
struct dprc_attributes {
	int container_id;
	/*!< Container's ID */
	uint16_t icid;
	/*!< Container's ICID */
	int portal_id;
	/*!< Container's portal ID */
	uint64_t options;
	/*!< Container's options as set at container's creation */
};

/**
 * @brief	Container configuration options, used in dprc_create_container()
 */
struct dprc_cfg {
	uint16_t icid;
	/*!< Container's ICID; if set to DPRC_GET_ICID_FROM_POOL, a free ICID
	 * will be allocated by the DPRC */
	uint64_t options;
	/*!< Combination of DPRC_CFG_OPT_ options */
};

/**
 * @brief   	Obtains the container id associated with a given portal.
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[out]	container_id	Requested container ID
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_container_id(struct dprc *dprc, int *container_id);

/**
 * @brief   	Opens a DPRC object for use and obtains its handle
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]	container_id	Container ID to open
 *
 * @returns	DPRC object handle, to be used in subsequent DPRC API calls
 *
 * @warning	Required before any operation on the object.
 */
int dprc_open(struct dprc *dprc, int container_id);

/**
 * @brief	Closes the DPRC object handle
 *
 * No further operations on the object are allowed after this call without
 * re-opening the object.
 *
 * @param[in]	dprc		DPRC descriptor object
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_close(struct dprc *dprc);

/**
 * @brief	Creates a child container
 *
 * @param[in]	dprc			DPRC descriptor object
 * @param[in]	cfg			Child container configuration
 * @param[out]	child_container_id	Child container ID
 * @param[out]	child_portal_paddr	Base physical address of the child portal
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_create_container(struct dprc *dprc,
                          struct dprc_cfg *cfg,
                          int *child_container_id,
                          uint64_t *child_portal_paddr);

/**
 * @brief	Destroys a child container.
 *
 * This function terminates the child container, so following this call the
 * child container ID becomes invalid.
 *
 * Notes:
 * - Destroying a container is allowed only if it contains no active devices.
 * - All resources of the destroyed container are returned to their
 *   original container.
 * - This function tries to destroy all the child containers of the specified
 *   container prior to destroying the container itself.
 *
 * @param[in]	dprc			DPRC descriptor object
 * @param[in]	child_container_id	ID of the container to destroy
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_destroy_container(struct dprc *dprc, int child_container_id);

/**
 * @brief	Sets allocation policy for a specific resource type in a child
 * 		container
 *
 * Allocation policy determines whether or not a container may allocate
 * resources from its parent. Each container has a 'global' allocation policy
 * that is set when the container is created.
 *
 * This function sets allocation policy for a specific resource type.
 * The default policy for all resource types matches the container's 'global'
 * allocation policy.
 *
 * @param[in]	dprc			DPRC descriptor object
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	res_type		Selects the resource/device type
 * @param[in]	quota			Sets the maximum number of resources of
 * 					the selected type that the child container
 * 					is allowed to allocate from its parent;
 * 					when quota is set to -1, the policy is
 * 					the same as container's general policy.
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Only the parent container is allowed to change a child policy.
 */
int dprc_set_res_quota(struct dprc *dprc,
                       int child_container_id,
                       uint16_t res_type,
                       uint16_t quota);

/**
 * @brief	Gets the allocation policy of a specific resource type in a \
 * 		child container
 *
 * @param[in]	dprc			DPRC descriptor object
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	res_type		Selects the resource/device type
 * @param[out]	quota			Holds the maximum number of resources of
 * 					the selected type that the child container
 * 					is allowed to allocate from the parent;
 * 					when quota is set to -1, the policy is
 * 					the same as container's general policy.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_quota(struct dprc *dprc,
                       int child_container_id,
                       uint16_t res_type,
                       uint16_t *quota);

/**
 * @brief	Resets a child container.
 *
 * In case a software context crashes or becomes non-responsive, the parent
 * may wish to reset its resources container before the software context is
 * restarted.
 *
 * This routine informs all devices assigned to the child container that the
 * container is being reset, so they may perform any cleanup operations that are
 * needed. All device handles that were owned by the child container shall be
 * closed.
 *
 * Note that such request may be submitted even if the child software context
 * has not crashed, but the resulting device cleanup operations will not be
 * aware of that.
 *
 * @param[in]	dprc			DPRC descriptor object
 * @param[in]	child_container_id	ID of the container to reset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_reset_container(struct dprc *dprc, int child_container_id);

/**
 * @brief   	Assigns devices or resource to a child container.
 *
 * Assignment is usually done by a parent (this DPRC) to one of its child
 * containers.
 *
 * According to the DPRC allocation policy, the assigned resources may be taken
 * (allocated) from the container's ancestors, if not enough resources are
 * available in the container itself.
 *
 * The type of assignment depends on the dprc_res_req options, as follows:
 * - DPRC_RES_REQ_OPT_EXPLICIT: indicates that assigned resources should have the
 *   explicit base ID specified at the id_base_align field of res_req.
 * - DPRC_RES_REQ_OPT_ALIGNED: indicates that the assigned resources should be
 *   aligned to the value given at id_base_align field of res_req.
 * - DPRC_RES_REQ_OPT_PLUGGED: Relevant only for device assignment, and indicates
 *   that the device must be set to the plugged state.
 *
 * If IRQ information has been set in the child DPRC, it will signal an
 * interrupt following every change in its device assignment.
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]	container_id 	ID of the child container
 * @param[in]	res_req		Describes the type and amount of resources to
 * 				assign to the given container.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_assign(struct dprc *dprc,
                int container_id,
                struct dprc_res_req *res_req);

/**
 * @brief	Un-assigns devices or resources from a child container
 * 		and moves them into this (parent) DPRC.
 *
 * Un-assignment of resources moves arbitrary or explicit resources
 * from the specified child container to the parent container.
 *
 * Un-assignment of devices can succeed only if the device is not in the
 * plugged or opened state.
 *
 * A container may use this function with its own ID in order to change a
 * device state to plugged or unplugged.
 *
 * @param[in]	dprc			DPRC descriptor object
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	res_req			Describes the type and amount of
 * 					resources to un-assign from the child
 * 					container
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_unassign(struct dprc *dprc,
                  int child_container_id,
                  struct dprc_res_req *res_req);

/**
 * @brief	Obtains the number of devices in the DPRC
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[out]	dev_count	Number of devices assigned to the DPRC
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_device_count(struct dprc *dprc, int *dev_count);

/**
 * @brief   	Obtains general information on a device
 *
 * @details	The device descriptors are retrieved one by one by incrementing
 * 		dev_index up to (not including) the value of dev_count returned
 * 		from dprc_get_device_count().
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]	dev_index	Index of the device to be queried (< dev_count)
 * @param[out]	dev_desc	Returns the requested device descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_device(struct dprc *dprc,
                    int dev_index,
                    struct dprc_dev_desc *dev_desc);

/**
 * @brief	Obtains the number of free  resources that are assigned
 * 		to this container, by resource type
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]	res_type	Resource type
 * @param[out]	res_count	Number of free resources of the given
 * 				resource type that are assigned to this DPRC
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_count(struct dprc *dprc,
                       uint16_t res_type,
                       int *res_count);

/**
 * @brief	Obtains IDs of free resources in the container
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]	res_type	Selects the resource type
 * @param[in,out] range_desc	range descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_ids(struct dprc *dprc,
                     uint16_t res_type,
                     struct dprc_res_ids_range_desc *range_desc);

/**
 * @brief   	Obtains container attributes
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[out]	attributes   	Container attributes
 *
 * @returns     '0' on Success; Error code otherwise.
 */
int dprc_get_attributes(struct dprc *dprc, struct dprc_attributes *attributes);

/**
 * @brief	Returns region information for a specified device.
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]	dev_type	Device type as returned in dprc_get_device()
 * @param[in]	dev_id		Unique device instance as returned in
 * 				dprc_get_device()
 * @param[in]	region_index	The specific region to query
 * @param[out]	region_desc	Returns the requested region descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_dev_region(struct dprc *dprc,
                        uint16_t dev_type,
                        uint16_t dev_id,
                        uint8_t region_index,
                        struct dprc_region_desc *region_desc);

/**
 * @brief 	Sets IRQ information for the DPRC.
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]	irq_index	Identifies the interrupt index to configure
 * @param[in]	irq_paddr	Physical IRQ address that must be written to
 * 				signal a message-based interrupt
 * @param[in]	irq_val		Value to write into irq_paddr address
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_set_irq(struct dprc *dprc,
                 uint8_t irq_index,
                 uint64_t irq_paddr,
                 uint32_t irq_val);

/**
 * @brief   	Gets IRQ information from the DPRC.
 *
 * @param[in]	dprc		DPRC descriptor object
 * @param[in]   irq_index	The interrupt index to configure;
 * 				DPRC supports only irq_index 0 - this interrupt
 * 				will be signaled on every change to resource/device
 * 				assignment in this DPRC.
 * @param[out]	irq_paddr	Physical address that must be written in order
 * 				to signal the message-based interrupt
 * @param[out]	irq_val		Value to write in order to signal the
 * 				message-based interrupt
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_irq(struct dprc *dprc,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val);

/*! @} */

#endif /* _FSL_DPRC_H */

