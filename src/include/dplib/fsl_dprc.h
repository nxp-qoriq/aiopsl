/* Copyright 2013 Freescale Semiconductor, Inc. */
/*!
 *  @file    fsl_dprc.h
 *  @brief   Data Path Resource Container API
 */

#ifndef _FSL_DPRC_H
#define _FSL_DPRC_H

#include "common/types.h"
#include "common/fsl_cmdif.h"


#ifdef MC
struct dprc;
#else
struct dprc {
	struct cmdif_desc cidesc;
};
#endif
/*!
 * @Group grp_dprc	Data Path Resource Container API
 *
 * @brief	Contains initialization APIs and runtime control APIs for DPRC
 * @{
 */

#define DPRC_GET_ICID_FROM_POOL		(uint16_t)(~(0))
/*!<
 * Set this value as the ICID value in dprc_create_attributes when creating a
 * container, in case the ICID is not selected by the user and should be
 * allocated by the DPRC from the pool of ICIDs
 */

/**
 * Resource types enumeration
 */
enum dp_res_type {
	DP_RES_TYPE_ICID,	//TODO - remove
	/*!< Isolation contexts ID */
	DP_RES_TYPE_MCPID,
	/*!< Management portals */
	DP_RES_TYPE_SWP,
	/*!< Software portals (base address + interrupt) */
	DP_RES_TYPE_BPID,
	/*!< Buffer pools */
	DP_RES_TYPE_CHANNELS,
	/*!< Channels */
	DP_RES_TYPE_FQID,
	/*!< Frame queues */
	DP_RES_TYPE_QPR,
	/*!< Queuing priority record */
	DP_RES_TYPE_QDID,
	/*!< Queuing destinations */
	DP_RES_TYPE_CGID,
	/*!< Congestion groups */
	DP_RES_TYPE_CEETM_LFQMTIDX,           
	/*!< Logical Frame Queue Mapping Table Index */
	DP_RES_TYPE_CEETM_DCTIDX,               
	/*!< Dequeue Command Type Index */
	DP_RES_TYPE_CEETM_CQCHID,                
	/*!< Class Queue Channel ID */
	DP_RES_TYPE_CEETM_LNIID,
	/*!< !Logical Network Interface ID */
	DP_RES_TYPE_TID,
	/*!< Classification tables */
	DP_RES_TYPE_KID,
	/*!< Key ID*/
	DP_RES_TYPE_PLPID,
	/*!< Policer profiles ID */
	DP_RES_TYPE_PRPID, 
	/*!< parser profile ID */
	DP_RES_TYPE_PPID,
	/*!< Physical ports */
	DP_RES_TYPE_IFPID,
	/*!< Interface profiles */
	DP_RES_TYPE_TASK,
	/*!< AIOP tasks*/
	DP_RES_TYPE_LOOKUP_TABLE,	//TODO - same as TID?
	/*!< Lookup tables */
	DP_RES_TYPE_RP_PORT,
	/*!< Recycle path port */
	DP_RES_TYPE_RPLR,
	/*!< Replication list record */
	DP_RES_TYPE_DPSW_PORT,
	/*!< DPSW port*/
	DP_RES_TYPE_DEVICE_DUMMY_FIRST,	//TODO - ignore, will be removed
	DP_RES_TYPE_DPNI,
	/*!< DPNI Device*/
	DP_RES_TYPE_DPIO,
	/*!< DPIO device */
	DP_RES_TYPE_DPSP,
	/*!< DPSP device */
	DP_RES_TYPE_DPSW,
	/*!< DPSW device */
	DP_RES_TYPE_DEVICE_DUMMY_LAST, 	//TODO - ignore, will be removed
	DP_RES_TYPE_DPRC,
	DP_RES_TYPE_MC
//TODO - can be removed?
};

/*!
 * @name Resource request options
 */
#define RES_REQ_OPT_EXPLICIT		0x00000001
/*!< Explicit resource id request - Relevant only for primitive resources
 * request. The requested resources are explicit and sequential The base ID is
 * given at res_req at base_align field */
#define RES_REQ_OPT_SEQUENTIAL		0x00000002
/*!< Sequential resources request - Relevant only for primitive resources
 * request. Indicates that resources id should be sequential and aligned to the
 * value given at dprc_res_req base_align field */
#define RES_REQ_OPT_PLUGGED		    0x00000004
/*!< Plugged Flag - Relevant only for device assignment request.
 * Indicates that after all devices assigned. An interrupt will be invoked at
 * the relevant GPP. The assigned device will be marked as plugged */
#define RES_REQ_OPT_SHARED		    0x00000008			//TODO - this should be internal only?
/*!< Shared flag - Relevant only for device assignment request.
 * In case this flag is set the device will be copied to the containers on
 * assignment otherwise it will be moved from its container to the new one */
/* @} */

/*!
 * @name Container general options
 */
#define DPRC_OPT_IOMMU_BYPASS		0x00000001
/*!<
 * Indicates a trusted container that is allowed to bypass IOMMU address
 * translations.
 */

/*!
 * @name Device Attributes Flags
 */
#define DPRC_DEV_STATE_OPEN		0x00000001
/*!< Opened state - Indicates that a device is opened for use by at least one owner */
#define DPRC_DEV_STATE_PLUGGED		0x00000002
/*!< Plugged state - Indicates that a device is plugged */
#define DPRC_DEV_STATE_SHARED		0x00000004
/*!< Shared state - Indicates that a device is shared by more than one container */
/* @} */

/**
 * @brief	Container spawn policy
 *
 * Spawn policy determines if the container is allowed to spawn or not.
 * A container (DPRC) may create child containers according to its spawn policy.
 */
enum dprc_spawn_policy {
	DPRC_SPAWN_POLICY_ALLOWED,
	/*!< Spawning is allowed */
	DPRC_SPAWN_POLICY_FORBIDDEN
/*!< Spawning is forbidden */
};

/**
 * @brief	Resource allocation policy
 *
 * Allocation policy defines the container permission to request resources from
 * its parent container.
 */
enum dprc_alloc_policy {
	DPRC_ALLOC_POLICY_UNLIMITED,
	/*!<
	 * Allocation from parent is unlimited. As long as resources are
	 * available, container can take as much resources as needed from the
	 * parent
	 */
	DPRC_ALLOC_POLICY_FORBIDDEN,
	/*!<
	 * Allocation from parent is forbidden
	 */
	DPRC_ALLOC_POLICY_BY_QUOTA,
	/*!<
	 * Allocation from parent is allowed but limited by a quota (has to be
	 * set by the parent)
	 */
	DPRC_ALLOC_POLICY_BY_CONTAINER_POLICY,
/*!<
 * Allocation from parent of a specific resource type is done according
 * to the 'global' policy set in the container. This option is valid only
 * when set to a specific resource type, and cannot be used by itself
 * as the container's global policy.
 */
};

/**
 * @brief	Resource request descriptor, to be used in assignment or
 * 		un-assignment of resources and devices.
 */
struct dprc_res_req {
	enum dp_res_type type;
	/*!< Resource type */
	uint32_t num;
	/*!< Number of resources */
	uint32_t options;
	/*!< Request options */
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
	/*!< Type of logical device resource */
	uint32_t id;
	/*!< ID of logical device resource */
	uint8_t rev_major;
	/*!< Major revision number */
	uint8_t rev_minor;
	/*!< Minor  revision number */
	uint32_t state;
	/*!< Device state */
	uint8_t irq_count;
	/*!< Number of interrupts supported by the device */
	uint8_t region_count;
/*!< Number of mappable regions supported by the device */
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
 * @brief	Container attributes, returned by dprc_get_attributes()
 */
struct dprc_attributes {
	int container_id;
	/*!< Container's ID */
	uint16_t icid;
	/*!< Container's ICID */
	uint16_t portal_id;
	/*!< Container's portal ID */
	enum dprc_spawn_policy spawn_policy;
	/*!< Container's spawn policy */
	enum dprc_alloc_policy allocation_policy;
	/*!< Container's default allocation policy for all its resources,
	 * primitives and devices*/
	uint32_t options;
/*!< Container's attributes flags */
};

/**
 * @brief	Container creation attributes, used in dprc_create_container()
 */
struct dprc_create_attributes {
	uint16_t icid;
	/*!< Container's ICID */
	enum dprc_spawn_policy spawn_policy;
	/*!< Container's spawn policy */
	enum dprc_alloc_policy alloc_policy;
	/*!< Container's default allocation policy for all its resources,
	 * primitives and devices*/
	uint32_t options;
/*!< Container's attributes flags */
};

/**
 * @brief   	Obtains the container id associated with a given portal.
 *
 * @param[in]	portal_vaddr	Pointer to MC portal registers
 * @param[out]	container_id	Requested container ID
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_container_id(struct dprc *dprc, int *container_id);

/**
 * @brief   	Opens a DPRC object for use and obtains its handle
 *
 * @param[in]	portal_vaddr	Pointer to MC portal registers
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
 * @param[in]	dprc	DPRC object handle
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_close(struct dprc *dprc);

/**
 * @brief	Creates a child container
 *
 * @param[in]	dprc			DPRC object handle
 * @param[in]	attributes		Child container attributes
 * @param[out]	child_container_id	Child container ID
 * @param[out]	child_portal_paddr	Base physical address of the child portal
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_create_container(struct dprc *dprc,
                          struct dprc_create_attributes *attributes,
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
 * - All primitive resources of the destroyed container are returned to their
 *   original container.
 * - This function tries to destroy all the child containers of the specified
 *   container prior to destroying the container itself.
 *
 * @param[in]	dprc			DPRC object handle
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
 * @param[in]	dprc			DPRC object handle
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	res_type		Selects the resource type
 * @param[in]	alloc_policy		Selects the allocation policy
 * @param[in]	quota			Sets the maximum number of resources of
 * 					the selected type that the child container
 * 					is allowed to allocate from the parent;
 * 					valid only if alloc_policy is set to
 * 					DPRC_ALLOC_POLICY_BY_QUOTA.
 *
 * @returns	'0' on Success; Error code otherwise.
 *
 * @warning	Only the parent container is allowed to change a child policy.
 */
int dprc_set_res_alloc_policy(struct dprc *dprc,
                              int child_container_id,
                              enum dp_res_type res_type,
                              enum dprc_alloc_policy alloc_policy,
                              uint16_t quota);

/**
 * @brief	Gets the allocation policy of a specific resource type in a \
 * 		child container
 *
 * @param[in]	dprc			DPRC object handle
 * @param[in]	child_container_id	ID of the child container
 * @param[in]	res_type		Selects the resource type
 * @param[out]	alloc_policy		Selects the allocation policy
 * @param[out]	quota			Sets the maximum number of resources of
 * 					the selected type that the child container
 * 					is allowed to allocate from the parent;
 * 					valid only if alloc_policy is set to
 * 					DPRC_ALLOC_POLICY_BY_QUOTA.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_alloc_policy(struct dprc *dprc,
                              int child_container_id,
                              enum dp_res_type res_type,
                              enum dprc_alloc_policy *alloc_policy,
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
 * @param[in]	dprc			DPRC object handle
 * @param[in]	child_container_id	ID of the container to reset
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_reset_container(struct dprc *dprc, int child_container_id);

/**
 * @brief   	Assigns devices or primitive resource to a child container.
 *
 * Assignment is usually done by a parent (this DPRC) to one of its child
 * containers.
 *
 * According to the DPRC allocation policy, the assigned resources may be taken
 * (allocated) from the container's ancestors, if not enough resources are
 * available in the container itself.
 *
 * The type of assignment depends on the dprc_res_req options, as follows:
 * - RES_REQ_OPT_EXPLICIT: indicates that assigned resources should have the 
 *   explicit base ID specified at the base_align field of res_req.
 * - RES_REQ_OPT_SEQUENTIAL: indicates that the assigned resources should be
 *   aligned to the value given at base_align field of dprc_res_req.
 * - RES_REQ_OPT_PLUGGED: Relevant only for device assignment, and indicates
 *   that the device must be set to the plugged state.
 *
 * If IRQ information has been set in the child DPRC, it will signal an
 * interrupt following every change in its resource assignment.
 *
 * @param[in]	dprc		DPRC object handle
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
 * @brief	Un-assigns devices or primitive resources from a child container
 * 		and moves them into this (parent) DPRC.
 *
 * Un-assignment of primitive resources moves arbitrary or explicit resources
 * from the specified child container to the parent container.
 *
 * Un-assignment of devices can succeed only if the device is not in the
 * plugged or opened state.
 *
 * A container may use this function with its own ID in order to change a
 * device state to plugged or unplugged.
 *
 * @param[in]	dprc			DPRC object handle
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
 * @param[in]	dprc		DPRC object handle
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
 * @param[in]	dprc		DPRC object handle
 * @param[in]	dev_index	Index of the device to be queried (< dev_count)
 * @param[out]	dev_desc	Returns the requested device descriptor
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_device(struct dprc *dprc,
                    int dev_index,
                    struct dprc_dev_desc *dev_desc);

/**
 * @brief	Obtains the number of free primitive resources that are assigned
 * 		to this container, by resource type
 *
 * @param[in]	dprc		DPRC object handle
 * @param[in]	res_type	Primitive resource type
 * @param[out]	res_count	Number of free primitive resources of the given
 * 				resource type that are assigned to this DPRC
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_count(struct dprc *dprc,
                       enum dp_res_type res_type,
                       int *res_count);

/**
 * @brief	Obtains IDs of free primitive resources in the container
 *
 * @param[in]	dprc		DPRC object handle
 * @param[in]	res_type	Selects the resource type
 * @param[in]	res_ids_num	The number of resource IDs to retrieve;
 * 				Indicates the size of res_ids array.
 * @param[out]	res_ids		Array of primitive resources IDs to fill
 * @param[out]	valid_count	Number of valid resource IDs returned - may be
 * 				less than the requested res_ids_num in case it
 * 				exceeds the number of available resources.
 *
 * @returns	'0' on Success; Error code otherwise.
 */
int dprc_get_res_ids(struct dprc *dprc,
                     enum dp_res_type res_type,
                     int res_ids_num,
                     uint32_t *res_ids,
                     int *valid_count);

/**
 * @brief   	Obtains container attributes
 *
 * @param[in]	dprc		DPRC object handle
 * @param[out]	attributes   	- Container attributes
 *
 * @returns     '0' on Success; Error code otherwise.
 */
int dprc_get_attributes(struct dprc *dprc, struct dprc_attributes *attributes);

/**
 * @brief	Returns region information for a specified device.
 *
 * @param[in]	dprc		DPRC object handle
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
 * @param[in]	dprc		DPRC object handle
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
 * @param[in]	dprc		DPRC object handle
 * @param[in]   irq_index	The interrupt index to configure (< irq_count)
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

