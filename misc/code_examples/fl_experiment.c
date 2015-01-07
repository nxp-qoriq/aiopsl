/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_ldpaa.h"
#include "fsl_fdma.h"
#include "fsl_ipf.h"
#include "general.h"

#define NUMBER_OF_FLE_TO_PRESENT	4

struct fl_iterator{
	struct ldpaa_fd fdl;
	struct ldpaa_fd list[NUMBER_OF_FLE_TO_PRESENT];
	int      index;
	uint8_t  seg_handle;
	uint8_t  fdl_handle;
	uint8_t  pad[2];
};

int32_t fl_iterator_init_and_get_first_frame(struct fl_iterator *fl_iter);
int32_t fl_iterator_init_and_get_first_frame(struct fl_iterator *fl_iter){
	struct fdma_present_frame_params params;
	fl_iter->fdl = *((struct ldpaa_fd *)(HWC_FD_ADDRESS)); /*copy FDL frame to fdl */

	params.asa_size = 0;
	params.fd_src =  &fl_iter->fdl;
	params.flags = 0;
	params.present_size = NUMBER_OF_FLE_TO_PRESENT * sizeof(struct ldpaa_fd);
	params.pta_dst = (void *)PRC_PTA_NOT_LOADED_ADDRESS;
	params.seg_dst = fl_iter->list;
	params.seg_offset = 0;

	fdma_present_frame(&params);  /* Present number of FLEs from the frame list */
	*((struct ldpaa_fd *)(HWC_FD_ADDRESS)) = fl_iter->list[0];  /* copy first FLE to default frame */
	fdma_present_default_frame(); /* Present the frame + header */
	fl_iter->index = 0;
	return 0;
}

fl_update_and_get_next(struct fl_iterator *fl_iter);
fl_update_and_get_next(struct fl_iterator *fl_iter) {
	uint16_t seg_length;
	fdma_store_default_frame_data();
	fl_iter->list[fl_iter->index] = *((struct ldpaa_fd *)(HWC_FD_ADDRESS));
	if (((struct ldpaa_fd *)(HWC_FD_ADDRESS))->flc){ //should check the f bit instead
		fdma_modify_default_segment_data(0, NUMBER_OF_FLE_TO_PRESENT * sizeof(struct ldpaa_fd)); // TODO gal non-default frame
		fdma_store_default_frame_data(); /* store frame list */ // TODO need STORE of non-default frame
		*((struct ldpaa_fd *)(HWC_FD_ADDRESS)) = fl_iter->fdl;
		return -1;
	}
	fl_iter->index++;
	if (fl_iter->index % NUMBER_OF_FLE_TO_PRESENT == (NUMBER_OF_FLE_TO_PRESENT-1)){
		fdma_modify_default_segment_data(0, NUMBER_OF_FLE_TO_PRESENT * sizeof(struct ldpaa_fd)); // TODO gal non-default frame
		fdma_present_default_frame_segment(0, fl_iter->list,
						   (uint16_t)fl_iter->index * sizeof(struct ldpaa_fd),
						   (uint16_t)(NUMBER_OF_FLE_TO_PRESENT * sizeof(struct ldpaa_fd)) /*present size*/,
						   &seg_length,
						   &fl_iter->seg_handle); // TODO gal non-default frame
	}
	*((struct ldpaa_fd *)(HWC_FD_ADDRESS)) = fl_iter->list[fl_iter->index];  // copy 1 FLE to default fd location 
	fdma_present_default_frame();  //Present the frame + header 
	return 0;
}

#define HM1(...) (0==0)
#define HM2(...) (0==0)
#define HM3(...) (0==0)
#define IPSEC(...) (0==0)

int32_t is_frame_list(struct ldpaa_fd * fd);
int32_t is_frame_list(struct ldpaa_fd * fd){
	return (LDPAA_FD_GET_FMT(fd) == 1);  
}

// Assuming SEC support in FL
int main1();
int main1()
{
	struct fl_iterator fl;
	int mtu = 1024;
	struct ipf_context ipf_ctx;
	int32_t fl_update_stat = 0;
	int32_t fl_get_next_stat = 0;
	int frame_list_flag;
	int hagit;
	/* ... Flow ... */
	
	/* ... Some more Flow ... */
	if (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) > mtu)
		ipf_generate_frag(&ipf_ctx); /* Frame List FD is now in task defaults, closed */
	IPSEC(...);
	frame_list_flag = is_frame_list(((struct ldpaa_fd *)(HWC_FD_ADDRESS)));
	if(frame_list_flag){
		fl_iterator_init_and_get_first_frame(&fl);
	}
	do{
		HM1(...);
		HM2(...);
		HM3(...);
		hagit = 1000000;
	} while (frame_list_flag && !(fl_update_stat = fl_update_and_get_next(&fl)));
	if (fl_update_stat){
		/* Error handling*/
	}
	if(fl_get_next_stat){
		/* Error handling*/
	}
	return hagit;
}

// Assuming SEC support in FL
int main2();
int main2()
{
	struct fl_iterator fl;
	int mtu = 1024;
	struct ipf_context ipf_ctx;
	int32_t fl_update_stat = 0;
	int32_t fl_get_next_stat = 0;
	int frame_list_flag;
	int doron;
	/* ... Flow ... */
	
	/* ... Some more Flow ... */
	if (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) > mtu)
		ipf_generate_frag(&ipf_ctx); /* Frame List FD is now in task defaults, closed */
	frame_list_flag = is_frame_list(((struct ldpaa_fd *)(HWC_FD_ADDRESS)));
	if(frame_list_flag){
		fl_iterator_init_and_get_first_frame(&fl);
	}
	do{
		
		HM1(...);
		doron = 1000000;
	} while (frame_list_flag && !(fl_update_stat = fl_update_and_get_next(&fl)));
	if (fl_update_stat){
		/* Error handling*/
	}
	if(fl_get_next_stat){
		/* Error handling*/
	}
	IPSEC(...);
	if(frame_list_flag){
		fl_iterator_init_and_get_first_frame(&fl);
	}
	do{
		HM2(...);
		HM3(...);
		doron = 1000000;
	} while (frame_list_flag && !(fl_update_stat = fl_update_and_get_next(&fl)));
	if (fl_update_stat){
		/* Error handling*/
	}
	if(fl_get_next_stat){
		/* Error handling*/
	}
	return 0;
}


