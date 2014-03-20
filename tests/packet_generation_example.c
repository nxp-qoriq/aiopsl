#include "dplib/fsl_fdma.h"

#define PACKET_NUM 100
#define PACKET_SIZE 60

/*
 * A simple example of creating 100 packets and enqueue them according to a
 * specified fqid and spid.
 * This example uses FDMA commands.
 * In case one of the FDMA commands has failed, the packet generation will be
 * stopped, and the failure status will be returned to the user.
 * */
int32_t packet_generation_example(uint32_t fqid, uint8_t spid);

int32_t packet_generation_example(uint32_t fqid, uint8_t spid)
{
	/* Generated frame FD.
	 * Must be aligned to 32 bytes according to fdma_create_frame()
	 * restrictions. */
	struct ldpaa_fd fd __attribute__((aligned(sizeof(struct ldpaa_fd))));
	uint8_t frame_handle; /* Frame handle for FDMA */
	char data[PACKET_SIZE]; /* Workspace buffer */
	int32_t status; /* Returned status */
	char i;

	/* fill in data for the generated packets */
	for (i = 0; i < PACKET_SIZE; i++)
		data[i] = i;
	/* create & enqueue packets */
	for (i = 0; i < PACKET_NUM; i++){
		/* create a new packet */
		status = fdma_create_frame(&fd, data, PACKET_SIZE,
				&frame_handle);
		if (status != FDMA_SUCCESS)
			return status;
		/* enqueue the generated packet */
		status = fdma_store_and_enqueue_frame_fqid(frame_handle,
				FDMA_ENWF_NO_FLAGS, fqid, spid);
		if (status != FDMA_SUCCESS)
			return status;
	}

	return status;
}
