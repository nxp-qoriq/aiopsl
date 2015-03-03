/**************************************************************************//*
 @File          log.c

 @Description   library implementation

 more_detailed_description

 @Cautions      None.
 *//***************************************************************************/

#include "log.h"

/*******************************************************************
 	 	 	 	 	 Global Variables
 *******************************************************************/
uint64_t g_log_buf_phys_address;
uint32_t g_log_buf_size;
uint32_t g_log_buf_count;
uint8_t  g_log_buffer_lock;


extern struct aiop_init_info g_init_data;
extern struct icontext icontext_aiop;
/*******************************************************************
 	 	 	 	 	 Static Functions
 *******************************************************************/

int log_init()
{
	char str[] = LOG_INIT_SIGN;
	struct icontext ic;
	
	g_log_buf_phys_address = g_init_data.sl_info.log_buf_paddr;	
	g_log_buf_size = g_init_data.sl_info.log_buf_size;
	g_log_buf_count = sizeof(str) - 1;
	g_log_buffer_lock = 0; 
	
	ASSERT_COND_LIGHT(g_log_buf_size > 1 * KILOBYTE);
	icontext_aiop_get(&ic);
	ASSERT_COND_LIGHT(ic.dma_flags);

	icontext_dma_write(&ic, (uint16_t)g_log_buf_count, str, g_log_buf_phys_address);
	return 0;
}

void log_print_to_buffer(char *str, uint16_t str_length)
{
	uint32_t local_counter;	
	uint16_t second_write_len;

	/*Save Accelerator Hardware Context*/
	/** Address for passing parameters to accelerators */
	uint32_t hwc1 = *((uint32_t *) HWC_ACC_IN_ADDRESS);
	/** Address for passing parameters to accelerators */
	uint32_t hwc2 = *((uint32_t *) HWC_ACC_IN_ADDRESS2);
	/** Address for passing parameters to accelerators */
	uint32_t hwc3 = *((uint32_t *) HWC_ACC_IN_ADDRESS3);
	/** Address for passing parameters to accelerators */
	uint32_t hwc4 = *((uint32_t *) HWC_ACC_IN_ADDRESS4);
	/** Address for reading results from accelerators (1st register) */
	uint32_t hwc5 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/** Address for reading results from accelerators (2nd register) */
	uint32_t hwc6 = *((uint32_t *) HWC_ACC_OUT_ADDRESS2);
	/** Address for reading reserved 1 from hardware accelerator context*/
	uint32_t hwc7 = *((uint32_t *) HWC_ACC_RESERVED1);
	/** Address for reading reserved 2 from hardware accelerator context*/
	uint32_t hwc8 = *((uint32_t *) HWC_ACC_RESERVED2);

	/*
	 * Adding END\n delimiter.
	 * The full str buffer size was given with 4 bytes extra for the end delimiter 
	 * */
	str[str_length++] = 'E'; 
	str[str_length++] = 'N';
	str[str_length++] = 'D';
	str[str_length++] = '\n';


	if(str_length > g_log_buf_size) /*in case the length is to big */
		return;
		

	
	lock_spinlock(&(g_log_buffer_lock));
	local_counter = g_log_buf_count;
	/*Move the pointer 4 bytes back */ 
	if(local_counter >= LOG_END_SIGN_LENGTH){
		/*Start to write from the last end of write without END delimiter*/
		local_counter -= LOG_END_SIGN_LENGTH;
	}
	else{/*cyclic write needed*/
		/*write from the end of the buffer (END\n delimiter is in the end of buffer)*/
		local_counter = g_log_buf_size + (local_counter - LOG_END_SIGN_LENGTH);
	}
	

	/*Fast phase*/
	if(local_counter + str_length <= g_log_buf_size )/*last position + string length + END delimiter*/
	{
		/* Enough buffer*/
		g_log_buf_count = local_counter + str_length;
		/*unlock spinlock*/
		unlock_spinlock(&(g_log_buffer_lock));
		icontext_dma_write(&icontext_aiop, str_length, str, g_log_buf_phys_address + local_counter);
	}
	else /*cyclic write needed*/
	{
		/*Two writes needed - calculate the second write*/		
		second_write_len = str_length - (uint16_t)(g_log_buf_size - local_counter);		
		str_length -= second_write_len; /*str_length is now first_write length*/
		g_log_buf_count = second_write_len; /*The counter will point to the end of the string from the beginning of buffer*/
		unlock_spinlock(&(g_log_buffer_lock));
		icontext_dma_write(&icontext_aiop, str_length, str, g_log_buf_phys_address + local_counter);
		str += str_length;
		icontext_dma_write(&icontext_aiop, second_write_len, str, g_log_buf_phys_address );		
	}
	
	/*Restore Accelerator Hardware Context*/
	/** Address for passing parameters to accelerators */
	*((uint32_t *) HWC_ACC_IN_ADDRESS) = hwc1;
	/** Address for passing parameters to accelerators */
	*((uint32_t *) HWC_ACC_IN_ADDRESS2) = hwc2;
	/** Address for passing parameters to accelerators */
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = hwc3;
	/** Address for passing parameters to accelerators */
	*((uint32_t *) HWC_ACC_IN_ADDRESS4) = hwc4;
	/** Address for reading results from accelerators (1st register) */
	*((uint32_t *) HWC_ACC_OUT_ADDRESS) = hwc5;
	/** Address for reading results from accelerators (2nd register) */
	*((uint32_t *) HWC_ACC_OUT_ADDRESS2) = hwc6;
	/** Address for reading reserved 1 from hardware accelerator context*/
	*((uint32_t *) HWC_ACC_RESERVED1) = hwc7;
	/** Address for reading reserved 2 from hardware accelerator context*/
	*((uint32_t *) HWC_ACC_RESERVED2) = hwc8;
}

