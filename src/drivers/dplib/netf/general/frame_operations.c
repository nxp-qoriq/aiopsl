/**************************************************************************//**
@File		fsl_fdma.h

@Description	This file contains the AIOP SW FDMA API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "common/types.h"
#include "common/errors.h"
#include "dplib/fsl_frame_operations.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "fdma.h"

int32_t create_frame(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t *frame_handle)
{
	struct fdma_present_frame_params present_frame_params;
	struct fdma_insert_segment_data_params insert_params;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	int32_t status;

	/* *fd = {0};*/
	fd->addr = 0;
	fd->control = 0;
	fd->flc = 0;
	fd->frc = 0;
	fd->length = 0;
	fd->offset = 0;

	if ((uint32_t)fd == HWC_FD_ADDRESS) {
		PRC_SET_ASA_SIZE(0);
		PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
		PRC_SET_SEGMENT_LENGTH(0);
		PRC_SET_SEGMENT_OFFSET(0);
		PRC_RESET_NDS_BIT();
		PRC_RESET_SR_BIT();
		/* present frame with empty segment */
		fdma_present_default_frame();
		/* Insert data to the frame */
		fdma_insert_default_segment_data(0, data, size, 0);
		/* Update segment address and length in the presentation
		 * context */
		PRC_SET_SEGMENT_ADDRESS((uint32_t)data);
		if (size > DEFAULT_SEGMENT_SIZE)
			size = DEFAULT_SEGMENT_SIZE;
		PRC_SET_SEGMENT_LENGTH(size);
		/* Re-run parser */
		status = parse_result_generate_default(0);
		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;
		*frame_handle = PRC_GET_FRAME_HANDLE();
		return status;
	} else {
		present_frame_params.fd_src = (void *)fd;
		present_frame_params.asa_size = 0;
		present_frame_params.flags = FDMA_INIT_NO_FLAGS;
		present_frame_params.pta_dst = (void *)
				PRC_PTA_NOT_LOADED_ADDRESS;
		present_frame_params.present_size = 0;
		present_frame_params.seg_offset = 0;

		fdma_present_frame(&present_frame_params);

		insert_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
		insert_params.frame_handle = present_frame_params.frame_handle;
		insert_params.from_ws_src = data;
		insert_params.insert_size = size;
		insert_params.seg_handle = present_frame_params.seg_handle;
		insert_params.to_offset = 0;

		fdma_insert_segment_data(&insert_params);

		*frame_handle = present_frame_params.frame_handle;

		return SUCCESS;
	}
}

int32_t create_fd(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size)
{
	struct fdma_present_frame_params present_frame_params;
	struct fdma_insert_segment_data_params insert_params;
	struct fdma_amq amq;
	uint8_t spid;

	/* *fd = {0};*/
	fd->addr = 0;
	fd->control = 0;
	fd->flc = 0;
	fd->frc = 0;
	fd->length = 0;
	fd->offset = 0;

	if ((uint32_t)fd == HWC_FD_ADDRESS) {
		PRC_SET_ASA_SIZE(0);
		PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
		PRC_SET_SEGMENT_LENGTH(0);
		PRC_SET_SEGMENT_OFFSET(0);
		PRC_RESET_NDS_BIT();
		fdma_present_default_frame();

		fdma_insert_default_segment_data(0, data, size,
				FDMA_REPLACE_SA_CLOSE_BIT);

		return fdma_store_default_frame_data();
	} else {
		present_frame_params.fd_src = (void *)fd;
		present_frame_params.asa_size = 0;
		present_frame_params.flags = FDMA_INIT_NO_FLAGS;
		present_frame_params.pta_dst = (void *)
				PRC_PTA_NOT_LOADED_ADDRESS;
		present_frame_params.present_size = 0;
		present_frame_params.seg_offset = 0;

		fdma_present_frame(&present_frame_params);

		insert_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
		insert_params.frame_handle = present_frame_params.frame_handle;
		insert_params.from_ws_src = data;
		insert_params.insert_size = size;
		insert_params.seg_handle = present_frame_params.seg_handle;
		insert_params.to_offset = 0;

		fdma_insert_segment_data(&insert_params);

		spid = *((uint8_t *)HWC_SPID_ADDRESS);
		return fdma_store_frame_data(present_frame_params.frame_handle,
				spid, &amq);
	}
}

void create_arp_request(
		struct ldpaa_fd *fd,
		uint8_t *sender_hw_addr,
		uint32_t sender_ip,
		uint32_t dest_ip,
		uint8_t *frame_handle)
{
	 /*TODO - implement*/
}
