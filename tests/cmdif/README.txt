=================================
CMDIF files to copy from <ldpaa-aiop-sl-tag>:
=================================
cmdif.h              	aiopsl\src\arch\ppc\platform\ls2085a_gpp   	  
fsl_cmdif_client.h   	aiopsl\src\include\kernel                  
fsl_cmdif_server.h   	aiopsl\src\include\kernel                  	  	  
fsl_cmdif_fd.h       	aiopsl\src\include\kernel                  	  
fsl_cmdif_flib_c.h   	aiopsl\src\include\kernel                  	  
fsl_cmdif_flib_s.h   	aiopsl\src\include\kernel                  	  
cmdif_client.h       	aiopsl\src\kernel         	  
cmdif_srv.h          	aiopsl\src\kernel                          	  
cmdif_client_flib.c     aiopsl\src\kernel                              	  
cmdif_srv_flib.c        aiopsl\src\kernel                              	  

=================================
Test Files:
=================================
mc\tests\cmdif_gpp\srv\cmdif_srv_test.c           - Example for Server test (based MC).
nadk_develop\apps\cmdif_demo\cmdif_server_demo.c  - Example for GPP Server test. To be tested with cmdif_integ_dbg.elf AIOP test.
nadk_develop\apps\cmdif_demo\cmdif_client_demo.c  - Example for GPP Client test. To be tested with cmdif_integ_dbg.elf AIOP test. 
aiopsl\build\aiop_sim\tests\cmdif_test\integ_out\cmdif_integ_dbg.elf - This is AIOP elf to be used for CMDIF tests.
aiopsl\tests\cmdif\cmdif_integration_test.c                          - Source code of cmdif_integ_dbg.elf.

What's new:
------------
1. CMDIF API and FLIBS have been updated to support virtual addresses
2. AIOP client and Server have been updated to support SMMU enabled.
3. Added isolation context dependent API, see fsl_icontext.h.
   This is the API to be used on AIOP for accessing GPP buffers -
   read/write/acquire/release.

=================================
GPP client side:
=================================

How to test:
------------
Run cmdif_integ_dbg.elf. This elf includes AIOP server & client.
Use mc.itb for MC firmaware.
Use module name "TEST0", like in the example below:
err = cmdif_open(cidesc, "TEST0", 0, async_cb, (void *)ind, command_buffer, command_buffer_phys_addr, size);
Once commands have reached AIOP server you'll see some debug information and at the end you should see "PASSED open command".

Use cmdif_send() to test regular commands.
Once commands reached AIOP server you'll see some debug information and at the end you should see 
"PASSED Synchronous Command"/"PASSED Asynchronous Command" and etc.

=================================
GPP server side:
=================================

How to test:
------------
Run cmdif_integ_dbg.elf. This elf includes AIOP server & client.
Use AIOP server to trigger AIOP client as shown at cmdif_srv_test.c and cmdif_integration_test.c.
"TEST0" control callback will trigger AIOP client according to command id that is sent to it.
These are the commands that can be sent to AIOP module "TEST0".
#define OPEN_CMD	0x100  
/*!< Will trigger cmdif_open("IRA") on AIOP, first byte of the data should be GPP DPCI id. 
     If there is no data DPCI id 4 will be used as default. */
#define NORESP_CMD	0x101  
/*!< Will trigger cmdif_send() on AIOP  which should reach registered module "IRA" on GPP server */
#define ASYNC_CMD	0x102  
/*!< Will trigger cmdif_send() on AIOP  which should reach registered module "IRA" on GPP server 
     which will send response to AIOP and call asyncronous cb on AIOP. 
     The activation of this command is a bit tricky as AIOP client needs a buffer for sending async commands. 
     cmdif_send(&cidesc, ASYNC_CMD, size, CMDIF_PRI_LOW, (uint64_t)async_buff, 
                      NULL, NULL);
     It is important that async_buff should be of size (size + AIOP_SYNC_BUFF_SIZE).
     AIOP will use (async_buff + size) as a buffer and AIOP_SYNC_BUFF_SIZE as a size for async command.
          
              async_buff
     ------------------------------------
    |    size      | AIOP_SYNC_BUFF_SIZE |
     ------------------------------------     		                 
		                 */
#define IC_TEST		0x106
/*!< Will trigger fsl_icontext.h test on AIOP, call it after cmdif_session_open() */

Server side test should have this code prior to sending commands to module TEST0:
	err |= cmdif_register("IRA", &ops);
	err |= cmdif_session_open(&cidesc[0], "IRA", 0, 30,
	                          buff, &auth_id);

I also share the code for cmdif_integ_dbg.elf, look at cmdif_integration_test.c on ctrl_cb0().
Here you'll see the activation of AIOP client.

