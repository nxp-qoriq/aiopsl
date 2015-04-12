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

/**
 * @file		cmdif_client_demo.c
 * @description A CMDIF demo application to show Command interface APIs
 *		usage for the applications using the AIOP device.
 */

/* System headers */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

/* NADK headers */
#include <nadk.h>
#include <nadk_aiop.h>
#include <nadk_types.h>
#include <nadk_time.h>
#include <nadk_common.h>
#include <nadk_dev.h>
#include <nadk_mbuf.h>
#include <nadk_cutil.h>
#include <nadk_mpool.h>
#include <fsl_cmdif_client.h>
#include <fsl_shbp.h>

/* The data size to transmit */
#define AIOP_DATA_SIZE 20
/* Data size required by applicaiton */
#define DATA_MEM_SIZE 0x200000 /* 2 MB, assumed for now */

#define TEST_NUM_PKTS 10
#define MAX_OPEN_CMDS 10
#define MAX_SHBPS     5
#define NUM_SHBP_BUFS 16

/* Data size required by applicaiton for shbp */
#define AIOP_SHBP_SIZE  SHBP_MEM_PTR_SIZE(NUM_SHBP_BUFS) 

/* Maximum number of tries for receiving the async reponse */
#define CMDIF_DEMO_NUM_TRIES	1000
/* Wait before each try (in milli-secconds) */
#define CMDIF_DEMO_ASYNC_WAIT	100

/* Macro to move to the next cmd-line arg and returns the value of argc */
#define ARGINC() ({ argv++; --argc; })

/* Command ID for testing shared pool where AIOP is allocation master */
#define SHBP_TEST	0x109

/* Command ID for testing shared pool where GPP is allocation master */
#define SHBP_TEST_GPP	0x110

/*
 * Structure for testing shared pool
 */
struct shbp_test {
	uint64_t shbp;
	uint8_t dpci_id;
};

/*
 * Structure to contains command line parameters.
 */
struct cmd_param {
	char		*vfio_grp;
	uint8_t		log_level;
	uint32_t	number_of_files;
	uint16_t	file_size;
	bool		console_logging;
};
/*
 * Command line parameters.
 */
struct cmd_param nadk_cmd_opts;

/* Each thread is represented by a "worker" struct. */
struct worker {
	pthread_t id;
	uint8_t cpu;
};

/* A global pointer to store the AIOP device handle */
struct nadk_dev *aiop_dev;
/* Number of IO contexts */
uint32_t io_context_num;

struct cmdif_desc cidesc[2];
int async_count1;
int async_count2;

void *aiop_open_cmd_mem_pool;
void *aiop_data_mem_pool;
void *aiop_shbp_mem_pool;

/* The IO worker function, required by main() for thread-creation */
static void *app_io_thread(void *__worker);

/*
 * Fucntion to parse and validate the command line parameters
 */
void nadk_cmdif_parse_cmd_param(int argc, char **argv,
					struct cmd_param *cmd)
{
	while (ARGINC() > 0) {
		if (!strcmp(*argv, "-g")) {
			if (!ARGINC()) {
				printf("Missing argument to vfio_grp\n");
				exit(EXIT_FAILURE);
			}
			cmd->vfio_grp = *argv;
		}
		if (!strcmp(*argv, "-l")) {
			if (!ARGINC()) {
				printf("Missing argument to log_level\n");
				exit(EXIT_FAILURE);
			}
			cmd->log_level = atoi(*argv);
		}
		if (!strcmp(*argv, "-lnf")) {
			if (!ARGINC()) {
				printf("Missing argument to number_of_files\n");
				exit(EXIT_FAILURE);
			}
			cmd->number_of_files = atoi(*argv);
		}
		if (!strcmp(*argv, "-lfs")) {
			if (!ARGINC()) {
				printf("Missing argument to file_size\n");
				exit(EXIT_FAILURE);
			}
			cmd->file_size = atoi(*argv);
		}
		if (!strcmp(*argv, "-lcl"))
			cmd->console_logging = true;

		if (!strcmp(*argv, "-h")) {
			printf("Usage is as follows:-\n\n");
			printf("./cmdif_client_demo -g <container name> -l <log level> -lnf <number of log files> -lfs <size of each log file> -lcl\n\n");
			printf("\t\t container name :- DPRC with which application is run\n");
			printf("\t\t log level :- The log level with which application is run.");
			printf("\t\t number of log files :- The number of files used by application to store logs.\n");
			printf("\t\t size of each log file :- The size of each file used to store logs.\n");
			printf("\t\t use -lcl option to enable display of logs on the console along with file logging.\n\n");
			printf("All the logs having level less than the input log level will be displayed.\n\n");
			printf("Valid Log level range is from 1 to 8\n\n");
			printf("The Definitions of Log Levels are:-\n");
			printf("\t\tLog Level 1 : Emergency Logs[System is unusable]\n");
			printf("\t\tLog Level 2 : Alert Logs[Action must be taken immediately]\n");
			printf("\t\tLog Level 3 : Critical Logs[Critical conditions]\n");
			printf("\t\tLog Level 4 : Error Logs[Error conditions]\n");
			printf("\t\tLog Level 5 : Warning Logs[Warning conditions]\n");
			printf("\t\tLog Level 6 : Notice Logs[Normal but significant condition]\n");
			printf("\t\tLog Level 7 : Informational Logs[Informational]\n");
			printf("\t\tLog Level 8 : Debug Logs[Debug-level messages]\n\n");
			exit(EXIT_SUCCESS);
		}
	}
	return;
}


/* The main() function/thread creates the worker threads */
int main(int argc, char **argv)
{
	int32_t ret, num_threads;
	int32_t aiop_dev_found = 0;
	struct worker worker;
	struct nadk_init_cfg cfg;
	struct nadk_dev *dev;
	uint32_t max_rx_vq, max_tx_vq, i;

	/* Get VFIO Group ID String & other parameters from command line arguments */
	nadk_cmdif_parse_cmd_param(argc, argv, &nadk_cmd_opts);
	if (NULL == nadk_cmd_opts.vfio_grp)
		exit(EXIT_FAILURE);

	/* Initialize NADK */
	printf("Calling NADK Init with DPRC = %s\n", nadk_cmd_opts.vfio_grp);
	cfg.vfio_container = nadk_cmd_opts.vfio_grp;
	cfg.data_mem_size = DATA_MEM_SIZE;
	cfg.buf_mem_size = 0;
	cfg.log_level = nadk_cmd_opts.log_level;
	cfg.log_file_size = nadk_cmd_opts.file_size;
	cfg.log_files = nadk_cmd_opts.number_of_files;
	/* Not using SHARED MEMORY or USer Space Dispatcher */
	cfg.flags = 0;
	if(nadk_cmd_opts.console_logging)
		cfg.flags |= NADK_LOG_CONSOLE;

	ret = nadk_init(&cfg);
	if (NADK_FAILURE == ret) {
		printf("nadk_init failed\n");
		exit(EXIT_FAILURE);
	}

	/* Get Total available IO contexts */
	io_context_num = nadk_get_io_context_count();
	/* We need atleast 1 IO Context */
	if (io_context_num == 0) {
		NADK_ERR(APP1, "Not enough Resource to run");
		goto cleanup;
	}

	/* Get List of devices assigned to me */
	/* We need atleast one AIOP device for demo application to work */
	TAILQ_FOREACH(dev, &device_list, next) {
		switch (dev->dev_type) {
		case NADK_AIOP_CI:
			/* Get Max available RX & TX VQs for this device */
			max_rx_vq = nadk_dev_get_max_rx_vq(dev);
			if (max_rx_vq < 1) {
				NADK_ERR(APP1, "Not enough Resource to run");
				goto cleanup;
			}
			max_tx_vq = nadk_dev_get_max_tx_vq(dev);
			if (max_tx_vq < 1) {
				NADK_ERR(APP1, "Not enough Resource to run");
				goto cleanup;
			}

			/* Setup the Rx VQ */
			for (i = 0; i < max_rx_vq; i++) {
				ret = nadk_dev_setup_rx_vq(dev, i, FALSE, NULL);
				if (ret != NADK_SUCCESS) {
					NADK_ERR(APP1,
						"Setting up Rx VQ failed");
					goto cleanup;
				}
			}

			aiop_dev = dev;
			aiop_dev_found = 1;
			break;

		default:
			NADK_ERR(APP1, "ERROR: Unknown Device.");

		}

		/* We are looking for one AIOP device only */
		if (aiop_dev_found)
			break;
	}
	if (!aiop_dev_found) {
		NADK_ERR(APP1, "Not enough Resource to run");
		goto cleanup;
	}

	/** 1-Core Multi-threaded scenario. Use a Loop here if,
	we have need to create multiple threads **/
	/* Spawn single IO worker thread */
	worker.cpu = 0;
	ret = pthread_create(&worker.id, NULL,
			(void *(*)(void *))app_io_thread, &worker);
	if (ret) {
		NADK_ERR(APP1, "Fail: %s(%d)", "pthread_create", ret);
		exit(EXIT_FAILURE);
	} else
		num_threads = 1;

	/* Thread is created, now wait for thread to exit */
	while (num_threads) {
		if (!pthread_tryjoin_np(worker.id, NULL)) {
			NADK_INFO(APP1, "Exited: thread %d", worker.cpu);
			/* No more threads left, exit this loop too */
			num_threads = 0;
		}
	}

cleanup:
	/* Do cleanup and exit */
	nadk_cleanup();
	printf("Main Finished\n");
	return 0;
}

int async_cb1(void *async_ctx __always_unused, int err,
		uint16_t cmd_id __always_unused,
		uint32_t size, void *data)
{
	uint32_t j;
	uint8_t *v_data = (uint8_t *)(data);
	NADK_NOTE(APP1, "ASYNC CB data high = 0x%x low = 0x%x size = 0x%x",
		(uint32_t)(((uint64_t)data & 0xFF00000000) >> 32),
		(uint32_t)((uint64_t)data & 0xFFFFFFFF), size);
	/* Check for modified data from the AIOP server */
	for (j = 0; j < size; j++) {
		if ((v_data)[j] != 0xDA)
			NADK_ERR(APP1, "Invalid data from AIOP!!!");
	}
	if (err != 0)
		NADK_ERR(APP1, "ERROR inside async_cb");
	async_count1++;
	return err;
}

int async_cb2(void *async_ctx __always_unused, int err,
		uint16_t cmd_id __always_unused,
		uint32_t size, void *data)
{
	uint32_t j;
	uint8_t *v_data = (uint8_t *)(data);
	NADK_NOTE(APP1, "ASYNC CB data high = 0x%x low = 0x%x size = 0x%x",
		(uint32_t)(((uint64_t)data & 0xFF00000000) >> 32),
		(uint32_t)((uint64_t)data & 0xFFFFFFFF), size);
	/* Check for modified data from the AIOP server */
	for (j = 0; j < size; j++) {
		if ((v_data)[j] != 0xDA)
			NADK_ERR(APP1, "Invalid data from AIOP!!!");
	}
	if (err != 0)
		NADK_ERR(APP1, "ERROR inside async_cb");
	async_count2++;
	return err;
}

int create_global_mempools(void)
{
	struct nadk_mpool_cfg mpcfg;
	int ret;

	memset(&mpcfg, 0, sizeof(struct nadk_mpool_cfg));
	/* Fill all the params for the pool to be passed in cfg */
	mpcfg.name = "aiop_data_mem_pool";
	mpcfg.block_size = AIOP_DATA_SIZE;
	mpcfg.num_global_blocks = TEST_NUM_PKTS + (NUM_SHBP_BUFS * 2) + 1;
	mpcfg.num_max_blocks = mpcfg.num_global_blocks;

	/* Create the memory pool of the dma'ble memory */
	aiop_data_mem_pool = nadk_mpool_create(&mpcfg, NULL, NULL);
	if (!aiop_data_mem_pool) {
		NADK_ERR(APP1, "Unable to allocate the aiop data"
			"memory pool");
		return NADK_FAILURE;
	}

	memset(&mpcfg, 0, sizeof(struct nadk_mpool_cfg));
	/* Fill all the params for the pool to be passed in cfg */
	mpcfg.name = "aiop_open_cmd_mem_pool";
	mpcfg.block_size = CMDIF_OPEN_SIZE;
	mpcfg.num_global_blocks = MAX_OPEN_CMDS;
	mpcfg.num_max_blocks = MAX_OPEN_CMDS;

	/* Create the memory pool of the dma'ble memory */
	aiop_open_cmd_mem_pool = nadk_mpool_create(&mpcfg, NULL, NULL);
	if (!aiop_open_cmd_mem_pool) {
		NADK_ERR(APP1, "Unable to allocate the aiop open cmd "
			"memory pool");
		ret = nadk_mpool_delete(aiop_data_mem_pool);
		if (ret != NADK_SUCCESS)
			NADK_ERR(APP1, "Unable to delete the aiop data "
				"memory pool");
		return NADK_FAILURE;
	}

	memset(&mpcfg, 0, sizeof(struct nadk_mpool_cfg));
	/* Fill all the params for the pool to be passed in cfg */
	mpcfg.name = "aiop_shbp_mem_pool";
	mpcfg.block_size = AIOP_SHBP_SIZE;
	mpcfg.num_global_blocks = MAX_SHBPS;
	mpcfg.num_max_blocks = MAX_SHBPS;
	mpcfg.alignment = 64;

	/* Create the memory pool of the dma'ble memory */
	aiop_shbp_mem_pool = nadk_mpool_create(&mpcfg, NULL, NULL);
	if (!aiop_shbp_mem_pool) {
		NADK_ERR(APP1, "Unable to allocate the aiop open cmd "
			"memory pool");
		ret = nadk_mpool_delete(aiop_open_cmd_mem_pool);
		if (ret != NADK_SUCCESS)
			NADK_ERR(APP1, "Unable to delete the aiop open "
				"memory pool");
		ret = nadk_mpool_delete(aiop_data_mem_pool);
		if (ret != NADK_SUCCESS)
			NADK_ERR(APP1, "Unable to delete the aiop data "
				"memory pool");
		return NADK_FAILURE;
	}
	
	return NADK_SUCCESS;
}

int destroy_global_mempools(void)
{
	/* The below API is currently not implemented */
	nadk_mpool_delete(aiop_open_cmd_mem_pool);
	nadk_mpool_delete(aiop_data_mem_pool);

	return NADK_SUCCESS;
}

static int open_for_cmdif(uint64_t ind)
{
	uint8_t *data;
	int ret = 0;

	data = nadk_mpool_getblock(aiop_open_cmd_mem_pool, NULL);
	if (!data) {
		NADK_ERR(APP1, "Unable to get the memory");
		return NADK_FAILURE;
	}

	NADK_NOTE(APP1, "executing cmdif_open...");
	/* cidesc->regs is required to be set to NADK device */
	cidesc[ind].regs = (void *)aiop_dev;
	ret = cmdif_open(&cidesc[ind], "TEST0", 0,
		(ind&1) == 0 ? async_cb1 : async_cb2,
		(void *)ind, /* void *async_ctx */
		data, /* void *data */
		CMDIF_OPEN_SIZE); /* uint32_t size */
	if (ret != NADK_SUCCESS) {
		NADK_ERR(APP1, "cmdif_open failed");
		goto err;
	}

	return NADK_SUCCESS;

err:
	ret = nadk_mpool_relblock(aiop_data_mem_pool, data);
	if (ret != NADK_SUCCESS)
		NADK_ERR(APP1, "Error in releasing memory");
	return NADK_FAILURE;

}

static int close_for_cmdif(int ind)
{
	int ret = 0;

	NADK_NOTE(APP1, "executing cmdif_close...");
	ret = cmdif_close(&cidesc[ind]);
	return ret;
}

#define DUMP_SHBP(BP) \
	do {\
		NADK_NOTE(APP1,"bp->alloc_master = 0x%x", \
		          (BP)->alloc_master); \
		NADK_NOTE(APP1,"bp->max_num = 0x%x size = %d", \
		          (BP)->max_num, SHBP_SIZE(bp)); \
		NADK_NOTE(APP1,"bp->alloc.base high = 0x%x", \
		          (uint32_t)(((BP)->alloc.base & 0xFFFFFFFF00000000) >> 32)); \
		NADK_NOTE(APP1,"bp->alloc.base low = 0x%x", \
		          (uint32_t)((BP)->alloc.base & 0xFFFFFFFF)); \
		NADK_NOTE(APP1,"bp->alloc.deq = 0x%x", \
		          (BP)->alloc.deq); \
		NADK_NOTE(APP1,"bp->alloc.enq = 0x%x", \
		          (BP)->alloc.enq); \
		NADK_NOTE(APP1,"bp->free.base high = 0x%x", \
		          (uint32_t)(((BP)->free.base & 0xFFFFFFFF00000000) >> 32)); \
		NADK_NOTE(APP1,"bp->free.base low = 0x%x", \
		          (uint32_t)((BP)->free.base & 0xFFFFFFFF)); \
		NADK_NOTE(APP1,"bp->free.deq = 0x%x", (BP)->free.deq); \
		NADK_NOTE(APP1,"bp->free.enq = 0x%x\n", (BP)->free.enq); \
	} while(0)

static int shbp_test()
{
	int err = 0;
	int i;
	struct shbp *bp = NULL;
	void *data;
	uint8_t *mem_ptr;
	uint8_t *gpp_mem_ptr;
	struct shbp *gpp_bp = NULL;
	int ret = -1;
	uint64_t temp64;
	
	NADK_NOTE(APP1, "Testing SHBP");
	
	/**** AIOP SHBP ****/
	mem_ptr = nadk_mpool_getblock(aiop_shbp_mem_pool, NULL);
	if (!mem_ptr) {
		NADK_ERR(APP1, "Unable to get the memory");
		return NADK_FAILURE;
	}
	
	err = shbp_create(mem_ptr, AIOP_SHBP_SIZE, 0, &bp);
	if (err || (bp == NULL)) {
		NADK_ERR(APP1, "FAILED shbp_create bp %d", err);
		goto shbp_err0;
	}
	
	for (i = 0; i < NUM_SHBP_BUFS; i++) {
		data = nadk_mpool_getblock(aiop_data_mem_pool, NULL);
		if (!data) {
			NADK_ERR(APP1, "FAILED nadk_mpool_getblock for shbp");
			goto shbp_err1;
		}
		err = shbp_release(bp, data);
	}
	DUMP_SHBP(bp);
	
	NADK_NOTE(APP1, "Testing AIOP SHBP shbp_acquire");
	data = shbp_acquire(bp);
	if (data != NULL) {
		NADK_ERR(APP1, "FAILED shbp_acquire check for AIOP SHBP");
	}

	NADK_NOTE(APP1, "Testing AIOP SHBP shbp_refill");
	err = shbp_refill(bp);
	if (err != 0) {
		NADK_ERR(APP1, "FAILED shbp_refill check for AIOP SHBP");
	}
	DUMP_SHBP(bp);

	/***** GPP SHBP *****/
	gpp_mem_ptr = nadk_mpool_getblock(aiop_shbp_mem_pool, NULL);
	if (!gpp_mem_ptr) {
		NADK_ERR(APP1, "FAILED Unable to get the memory");
		goto shbp_err2;
	}
	
	err = shbp_create(gpp_mem_ptr, AIOP_SHBP_SIZE, SHBP_GPP_MASTER, &gpp_bp);
	if (err || (gpp_bp == NULL)) {
		NADK_ERR(APP1, "FAILED shbp_create gpp_bp %d", err);
		goto shbp_err2;
	}
	
	for (i = 0; i < NUM_SHBP_BUFS; i++) {
		data = nadk_mpool_getblock(aiop_data_mem_pool, NULL);
		if (!data) {
			NADK_ERR(APP1, "FAILED nadk_mpool_getblock for shbp");
			goto shbp_err3;
		}
		err = shbp_release(gpp_bp, data);
		if (err) {
			NADK_ERR(APP1, "FAILED shbp_release for GPP %d ", i);
			goto shbp_err3;			
		}
	}
	DUMP_SHBP(gpp_bp);
	
	NADK_NOTE(APP1, "Testing GPP SHBP shbp_acquire");
	data = shbp_acquire(gpp_bp);
	if (data == NULL) {
		NADK_ERR(APP1, "FAILED shbp_acquire check for GPP SHBP");
	}
	for (i = 0; i < AIOP_DATA_SIZE; i++) {
		((uint8_t *)data)[i] = i;
		if (((uint8_t *)data)[i] != i)
			NADK_ERR(APP1, "FAILED read/write data to shbp");
	}
	
	err = shbp_release(gpp_bp, data);
	if (err) {
		NADK_ERR(APP1, "FAILED shbp_release check for GPP SHBP");
	}
	DUMP_SHBP(gpp_bp);

	/*** Send it to AIOP ****/
	
	/*** AIOP SHBP ***/
	data = shbp_acquire(gpp_bp);
	if (!data) {
		NADK_ERR(APP1, "FAILED shbp_acquire from GPP SHBP");
		goto shbp_err3;
	}

	temp64 = 0;
	do {
		NADK_NOTE(APP1, "Sending AIOP SHBP...");
		((struct shbp_test *)data)->dpci_id = 
			(uint8_t)(get_aiop_dev_id(aiop_dev));
		((struct shbp_test *)data)->shbp = nadk_bswap64((uint64_t)bp);
		err = cmdif_send(&cidesc[0], 
		                 SHBP_TEST, 
		                 sizeof(struct shbp_test),
		                 CMDIF_PRI_LOW, 
		                 (uint64_t)(data));
		if (err) {
			NADK_ERR(APP1, "FAILED SHBP_TEST %p err = %d", 
			         data, err);
		} else {
			NADK_NOTE(APP1, "SHBP after cmdif_send()");
			if (((bp->alloc.deq % NUM_SHBP_BUFS) != 0) || 
				((bp->free.enq % NUM_SHBP_BUFS) != 0)) {
				NADK_ERR(APP1, 
				         "FAILED SHBP_TEST alloc.deq = %d", 
				         bp->alloc.deq);
				DUMP_SHBP(bp);
				goto shbp_err3;
			}
		}

		NADK_NOTE(APP1, "Testing AIOP SHBP shbp_refill");
		err = shbp_refill(bp);
		if (err != NUM_SHBP_BUFS) {
			NADK_ERR(APP1, 
			         "FAILED shbp_refill check for AIOP SHBP");
		}
		temp64++;
	} while (temp64 < 0x2); /* For massive testing change here */
	
	err = shbp_release(gpp_bp, data);
	if (err) {
		NADK_ERR(APP1, "FAILED shbp_release gpp_bp %p", data);
		goto shbp_err3;
	}


	/*** GPP SHBP ***/
	NADK_NOTE(APP1, "Sending GPP SHBP...");

	data = shbp_acquire(gpp_bp);
	if (!data) {
		NADK_ERR(APP1, "FAILED shbp_acquire from GPP SHBP");
		goto shbp_err3;
	}
	
	((struct shbp_test *)data)->dpci_id = 
		(uint8_t)(get_aiop_dev_id(aiop_dev));
	((struct shbp_test *)data)->shbp = nadk_bswap64((uint64_t)gpp_bp);
	err = cmdif_send(&cidesc[0], SHBP_TEST_GPP, sizeof(struct shbp_test),
	                 CMDIF_PRI_LOW, (uint64_t)(data));
	/* AIOP will set it to 0 */
	if (err || (((struct shbp_test *)data)->shbp != 0)) {
		NADK_ERR(APP1, "FAILED SHBP_TEST_GPP %p err = %d", data, err);
		err = shbp_release(gpp_bp, data);
		goto shbp_err3;
	}
	
	/* AIOP must release 1 buffer into this pool */
	NADK_NOTE(APP1, "Testing GPP SHBP shbp_refill");
	err = shbp_refill(gpp_bp);
	if (err != 1) {
		NADK_ERR(APP1, "FAILED shbp_refill check for AIOP SHBP");
		err = shbp_release(gpp_bp, data);
		goto shbp_err3;
	}

	NADK_NOTE(APP1, "PASSED Finished SHBP test");
	DUMP_SHBP(bp);
	DUMP_SHBP(gpp_bp);
	ret = 0;

shbp_err3:	
	i = 0;
	if (gpp_bp != NULL) {
		err = shbp_destroy(gpp_bp, &data);
		while(err && data) {
			err = nadk_mpool_relblock(aiop_data_mem_pool, data);
			if (err != NADK_SUCCESS)
				NADK_ERR(APP1, "Error in releasing memory");
			i++;
			err = shbp_destroy(gpp_bp, &data);
		}	
	}
	DUMP_SHBP(gpp_bp);
	NADK_NOTE(APP1, "Destroyed %d buffers", i);

shbp_err2:
	err = nadk_mpool_relblock(aiop_shbp_mem_pool, gpp_mem_ptr);
	if (err != NADK_SUCCESS)
		NADK_ERR(APP1, "Error in releasing memory");

shbp_err1:
	i = 0;
	if (bp != NULL) {
		err = shbp_destroy(bp, &data);
		while(err && data) {
			err = nadk_mpool_relblock(aiop_data_mem_pool, data);
			if (err != NADK_SUCCESS)
				NADK_ERR(APP1, "Error in releasing memory");
			i++;
			err = shbp_destroy(bp, &data);
		}
	}
	DUMP_SHBP(bp);
	NADK_NOTE(APP1, "Destroyed %d buffers", i);

shbp_err0:
	err = nadk_mpool_relblock(aiop_shbp_mem_pool, mem_ptr);
	if (err != NADK_SUCCESS)
		NADK_ERR(APP1, "Error in releasing memory");

	return ret;	
}

/* This is the worker IO thread function. */
static void *app_io_thread(void *__worker)
{
	struct worker *worker = __worker;
	uint32_t io_context_index;
	uint8_t *data;
	int ret;
	int ret1 = NADK_FAILURE, ret2 = NADK_FAILURE;
	uint8_t i, j;
	int t;

	ret = create_global_mempools();
	if (ret != NADK_SUCCESS) {
		NADK_ERR(APP1, "mempool creation failure");
		goto err1;
	}

	/* Use first available IO context */
	io_context_index = 1;
	ret = nadk_thread_affine_io_context(io_context_index);
	if (ret < 0) {
		/* Error indicates that NADK is not
		 * able to allocate IO Portal */
		NADK_ERR(APP1, "(%d): Fail: nadk_thread_affine_io_context",
			worker->cpu);
		goto err2;
	}

	/* This should be done only by first thread */
	if (worker->cpu == 0) {
		/* Start the device when the IO context is affined */
		ret = nadk_dev_start(aiop_dev);
		if (ret != NADK_SUCCESS) {
			NADK_ERR(APP1, "Error in starting device");
			goto err3;
		}
	}

	ret = open_for_cmdif(0);
	if (ret) {
		NADK_ERR(APP1, "Open for cmdif failed");
		goto err4;
	}

	ret = open_for_cmdif(1);
	if (ret) {
		NADK_ERR(APP1, "Open for cmdif failed");
		goto err5;
	}

	NADK_NOTE(APP1, "PASSED open commands");
	
	ret = 0;
	for (i = 0; i < 5; i++) {
		ret |= shbp_test();
	}
	if (ret == 0)
		NADK_NOTE(APP1, "PASSED SHBP tests %d", i);
	else
		NADK_NOTE(APP1, "FAILED SHBP tests %d", i);
	
	/* Get a memory block */
	/* NOTE: Here we are using the same memory and same block of the
	* mempool, but separate memory can also be used i.e. getblock can be
	* done in the below 'for' loop */
	data = nadk_mpool_getblock(aiop_data_mem_pool, NULL);
	if (!data) {
		NADK_ERR(APP1, "Unable to get the memory");
		goto err6;
	}

	NADK_NOTE(APP1, "Executing cmdif_send sync");
	for (i = 0; i < TEST_NUM_PKTS; i++) {
		for (j = 0; j < AIOP_DATA_SIZE; j++)
			data[j] = i+j;
		ret = cmdif_send(&cidesc[0], i, /* cmd_id */
				AIOP_DATA_SIZE, /* size */
				(i & 1), /* priority */
				(uint64_t)(data) /* data */);
		if (ret)
			NADK_ERR(APP1, "FAILED sync_send %d", i);
	}

	NADK_NOTE(APP1, "PASSED syncronous send commands");

	NADK_NOTE(APP1, "Executing cmdif_send async1");
	for (i = 0; i < 1; i++) {
		t = 0;

		for (j = 0; j < AIOP_DATA_SIZE; j++)
			data[j] = i+j;
		/* Multiple sends can be done without calling cmdif_resp_read */
		ret = cmdif_send(&cidesc[0], (i | CMDIF_ASYNC_CMD), /*cmd_id*/
				AIOP_DATA_SIZE, /* size */
				(i & 1), /* priority */
				(uint64_t)(data) /* data */);
		if (ret)
			NADK_ERR(APP1, "FAILED async_send %d", i);

		/* Now read the response */
		while (!async_count1 && (t < CMDIF_DEMO_NUM_TRIES)) {
			nadk_msleep(CMDIF_DEMO_ASYNC_WAIT);
			ret = cmdif_resp_read(&cidesc[0], (i & 1));
			if (ret)
				NADK_ERR(APP1, "FAILED cmdif_resp_read %d", i);
			t++;
		}
		if (!async_count1)
			NADK_ERR(APP1, "FAILED: asyncronous command");
	}

	NADK_NOTE(APP1, "Executing cmdif_send async2");
	for (i = 0; i < 1; i++) {
		t = 0;

		for (j = 0; j < AIOP_DATA_SIZE; j++)
			data[j] = i+j;
		/* Multiple sends can be done without calling cmdif_resp_read */
		ret = cmdif_send(&cidesc[1], (i | CMDIF_ASYNC_CMD), /*cmd_id*/
				AIOP_DATA_SIZE, /* size */
				(i & 1), /* priority */
				(uint64_t)(data) /* data */);
		if (ret)
			NADK_ERR(APP1, "FAILED async_send %d", i);

		/* Now read the response */
		while (!async_count2 && (t < CMDIF_DEMO_NUM_TRIES)) {
			nadk_msleep(CMDIF_DEMO_ASYNC_WAIT);
			ret = cmdif_resp_read(&cidesc[1], (i & 1));
			if (ret)
				NADK_ERR(APP1, "FAILED cmdif_resp_read %d", i);
			t++;
		}
		if (!async_count2)
			NADK_ERR(APP1, "FAILED: asyncronous command");
	}

	if (async_count1 && async_count2)
		NADK_NOTE(APP1, "PASSED assyncronous send/receive commands");

	/* Clean-up */
	ret = nadk_mpool_relblock(aiop_data_mem_pool, data);
	if (ret != NADK_SUCCESS)
		NADK_ERR(APP1, "Error in releasing memory");

err6:
	ret1 = close_for_cmdif(1);
	if (ret1 != NADK_SUCCESS)
		NADK_ERR(APP1, "FAILED: Close command");

err5:
	ret2 = close_for_cmdif(0);
	if (ret2 != NADK_SUCCESS)
		NADK_ERR(APP1, "FAILED: Close command");

	if (ret1 == NADK_SUCCESS && ret2 == NADK_SUCCESS)
		NADK_NOTE(APP1, "PASSED: close commands");

err4:
	/* This should be done only by first thread */
	if (worker->cpu == 0) {
		/* Do Gracefull Device Stop */
		ret = nadk_dev_stop(aiop_dev);
		if (ret != NADK_SUCCESS)
			NADK_ERR(APP1, "Error in stopping device");
	}

err3:
	/* Deaffine the IO context */
	nadk_thread_deaffine_io_context();

err2:
	ret = destroy_global_mempools();
	if (ret != NADK_SUCCESS)
		NADK_ERR(APP1, "Mempool deletion failure");
err1:
	NADK_NOTE(APP1, "(%d): Finished", worker->cpu);
	pthread_exit(NULL);
}

