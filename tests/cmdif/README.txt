=================================
CMDIF files to copy from ldpaa-aiop-sl-v0.4.1.update_02:
=================================
cmdif.h              	aiopsl\src\arch\ppc\platform\ls2100a_gpp   	  
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
cmdif_srv_mc.c          Example for server implementation
cmdif_srv_test.c        Example for server test
aiop_cmdif_integ.c      Source code of aiop_cmdif_integ.elf
aiop_cmdif_integ.elf    AIOP elf to be used for tests

=================================
GPP client side:
=================================
What's new:
------------
1. Added cmdif.h file which is architecture dependant.
2. Fixed endianess issue inside client flib.
   The fix does not include:
   	/* Convert into Big Endian for QBMAN/AIOP server */
	fd->u_flc.flc = rte_bswap64(fd->u_flc.flc);
   As it's not client FLIB related issue this should be added inside cmdif_client_nadk.c
3. AIOP server has been updated to support new DPCI (MC Alpha 0.4.2). 
   This means that AIOP root container should have DPCI objects that are linked to GPP dpci objects.
   See aiop_mc_int\misc\setup\dpl.dts layout example and create you own based on it.
   Without DPCI objects in DPL AIOP server will not be able to receive commands.
4. Note AIOP client and server require 2 different DPCI objects.
5. See documentation on ctrl_cb_t at aiopsl\src\include\kernel\fsl_cmdif_server.h
 
How to test:
------------
Run aiop_cmdif_integ.elf. This elf includes AIOP server & client.
Use MC Alpha 0.4.2 release.
Use module name "TEST0", like in the example below:
err = cmdif_open(cidesc, "TEST0", 0, async_cb, (void *)ind, command_buffer, command_buffer_phys_addr, size);
Once commands have reached AIOP server you'll see some debug information and at the end you should see "PASSED open command".

Use cmdif_send() to test regular commands.
Once commands reached AIOP server you'll see some debug information and at the end you should see 
"PASSED Synchronous Command"/"PASSED Asynchronous Command" and etc.

=================================
GPP server side:
=================================
What's new:
------------
1. CMDIF server flibs to be used to build real server.
2. Updated cmdif_srv_gpp.c reference design which shows the usage of server flibs.
   It has been tested on MC.
3. AIOP client application is up and it is part of aiop_cmdif_integ.elf.
4. Note AIOP client and server require 2 different DPCI objects.
5. cmdif_close() is not supported by AIOP client therefore the same applies for cmdif_session_close() on server side.
6. Synchronous commands are disabled on AIOP client.

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

Server side test should have this code prior to sending commands to module TEST0:
	err |= cmdif_register("IRA", &ops);
	err |= cmdif_session_open(&cidesc[0], "IRA", 0, 30,
	                          buff, &auth_id);

I also share the code for aiop_cmdif_integ.elf, look at aiop_cmdif_integ.c on ctrl_cb().
Here you'll see the activation of AIOP client.

