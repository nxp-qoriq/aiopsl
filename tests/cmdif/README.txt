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
   See dpl_example.dts, DPCI1 inside AIOP container is linked to DPCI0.
   Without these objects in DPL AIOP server will not be able to receive commands.
4. Note AIOP client and server require 2 different DPCI objects.
 
How to test:
------------
Run aiop_cmdif_integ.elf. This elf includes AIOP server & client.
Use MC Alpha 0.4.2 release that is going to be release on 26-June-2014.
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

How to test:
------------
Run aiop_cmdif_integ.elf. This elf includes AIOP server & client.
Use AIOP server to trigger AIOP client as shown at cmdif_srv_test.c and aiop_cmdif_integ.c.
"TEST0" control callback will trigger AIOP client according to command id that is sent to it.
These are the commands that can be sent to AIOP module "TEST0". Look at cmdif_srv_test.c in order to understand the right sequence.
#define OPEN_CMD	0x100
#define NORESP_CMD	0x101
#define ASYNC_CMD	0x102
#define SYNC_CMD 	0x103
I also share the code for aiop_cmdif_integ.elf, look at aiop_cmdif_integ.c on ctrl_cb().
Here you'll see the activation of AIOP client.

