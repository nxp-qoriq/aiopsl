#include "dplib/fsl_fdma.h"
#include "general.h"

#define PACKET_NUM 100
#define PACKET_SIZE 60

/*
 * A simple example of creating a frame from raw data and enqueue it.
 * */

#define PACKET_LENGTH		60
#define SEGMENT_PRES_ADDR	0x280
#define SEGMENT_PRES_SIZE	PACKET_LENGTH
#define SEGMENT_PRES_OFFSET	0

void create_frame_example(uint32_t fqid, uint16_t icid);

void create_frame_example(uint32_t fqid, uint16_t icid)
{
	uint8_t pkt[PACKET_LENGTH];
	struct ldpaa_fd  default_frame_fd
    	    	    __attribute__((aligned(sizeof(struct ldpaa_fd))));
	struct ldpaa_fd  new_fd
    	    	    __attribute__((aligned(sizeof(struct ldpaa_fd))));
	struct presentation_context *prc =
		    (struct presentation_context *) HWC_PRC_ADDRESS;

	uint8_t frame_handle, i;

	for (i = 0; i < PACKET_LENGTH; i++)
		pkt[i] = i;
	/* Store the default frame */
	fdma_store_default_frame_data();
	/* Save the updated FD (after the store) for future use */
	default_frame_fd = *((struct ldpaa_fd *)HWC_FD_ADDRESS);

	/* Case 1 - create a frame as the default frame - keep the frame open */
	fdma_create_frame((struct ldpaa_fd *)HWC_FD_ADDRESS, pkt,
		    PACKET_LENGTH, &frame_handle);
	/* Present a default data segment of the default frame.
	 * In this example we present 256 bytes from the beginning of the frame
	 * to workspace address 0x280 (we leave 0x80 headroom from the beginning
	 * of the presentation area(configured in the .lcf file))*/
	fdma_present_default_frame_segment(
			FDMA_PRES_NO_FLAGS,
			(void *)SEGMENT_PRES_ADDR,
			SEGMENT_PRES_OFFSET,
			SEGMENT_PRES_SIZE);

	/* ...
	 * Process the frame data
	 * ...*/

	/* Store and Enqueue the frame according to a given fqid */
	fdma_store_and_enqueue_default_frame_fqid(fqid, FDMA_ENWF_NO_FLAGS);
	/* Store and Enqueue the frame according to a given fqid in separate
	 * operations*/
	/*fdma_store_default_frame_data();
	fdma_enqueue_default_fd_fqid(icid, FDMA_ENF_NO_FLAGS, fqid);*/


	/*  Case 2 - create a frame (not as the default frame) -
	* This function closes the frame after creating it. */
	fdma_create_fd(&new_fd, pkt, PACKET_LENGTH);

	/* enqueue the frame according to a given fqid and icid */
	fdma_enqueue_fd_fqid(
		    &new_fd,
		    FDMA_ENF_BDI_BIT | FDMA_ENF_VA_BIT | FDMA_ENF_PL_BIT,
		    fqid,
		    icid);

	return;
}
