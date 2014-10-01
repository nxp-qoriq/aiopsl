=================================
CMDIF files to copy from aiop_phase_03_candidate_for_mc_SHA_3d00544:
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
Files inside this zip:
=================================
cmdif_srv_test.c        Example for GPP server test (based MC)
aiop_cmdif_integ.c      Source code of aiop_cmdif_integ.elf
aiop_cmdif_integ.elf    AIOP elf to be used for tests
mc.itb                  MC firmware to be used for testing cmdif

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
Run aiop_cmdif_integ.elf. This elf includes AIOP server & client.
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
Run aiop_cmdif_integ.elf. This elf includes AIOP server & client.
Use AIOP server to trigger AIOP client as shown at cmdif_srv_test.c and aiop_cmdif_integ.c.
"TEST0" control callback will trigger AIOP client according to command id that is sent to it.
These are the commands that can be sent to AIOP module "TEST0". Look at cmdif_srv_test.c in order to understand the right sequence.
#define OPEN_CMD	0x100  
/*!< Will trigger cmdif_open("IRA") on AIOP */
#define NORESP_CMD	0x101  
/*!< Will trigger cmdif_send() on AIOP  which should reach registered module "IRA" on GPP server */
#define ASYNC_CMD	0x102  
/*!< Will trigger cmdif_send() on AIOP  which should reach registered module "IRA" on GPP server 
     which will send response to AIOP and call asyncronous cb on AIOP */
#define IC_TEST		0x106
/*!< Will trigger fsl_icontext.h test on AIOP, call it after cmdif_session_open() */

Server side test should have this code prior to sending commands to module TEST0:
	err |= cmdif_register("IRA", &ops);
	err |= cmdif_session_open(&cidesc[0], "IRA", 0, 30,
	                          buff, &auth_id);

I also share the code for aiop_cmdif_integ.elf, look at aiop_cmdif_integ.c on ctrl_cb().
Here you'll see the activation of AIOP client.

