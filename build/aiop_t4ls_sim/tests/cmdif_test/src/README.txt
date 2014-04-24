The following file includes the instructions for cmdif_test demo.

This demo has 2 mode:
1. REFLECTOR_DEMO  
   This mode is set by #define REFLECTOR_DEMO inside cmdif_test.c.
   In this mode the server will receive asyncronious command and will send the 
   same FD as responce inro fqid as specified inside dequueue context. 
2. Client Server test
   This mode is set by undefining REFLECTOR_DEMO by 
   /* #define REFLECTOR_DEMO */ inside cmdif_test.c
   In this mode the server will receive variouse kinds of commands. 
   The user should follow the outprints for PASS/FAIL.

===============================================================================
REFLECTOR_DEMO user required settings
===============================================================================

FD[FLC] - frame descriptor special settings
-------------------------------------------
1. Set FD[FLC] bits 9-0 to 0x00, EPID entry index = 0x0 .
2. Set FD[FLC] bits 31-16 to 0x2000, it will make FLC[checksum] = 0x2000.
3. Set FD[FLC] bits 63-32 to 0x0, it will make FLC[hash] = 0x0.

===============================================================================
Client Server demo user required settings
===============================================================================

FD[FLC] - frame descriptor special settings
-------------------------------------------
1. Set FD[FLC] bits 9-0 to 0x00, EPID entry index = 0x0 .


===========================================
Demo possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. Define REFLECTOR_DEMO or undefine REFLECTOR_DEMO

===========================================
Execution flow
===========================================
1. Run the demo and get till the core is at waiting state
2. Start enqueueing FDs with FD[FLC] set as described above.
3. 
3.1 For REFLECTOR_DEMO see that cmdif_srv_isr() is called on every FD and 
    the FD is sent back according to Frame Queue Context as received from QMan.
   
    For Client Server demo see that app_receive_cb() is called on every FD.
    The demo counts FDs and simulates different commands according to this 
    number. 

=================
Important NOTEs:
=================
ARENA SW sets CTSCSR0[CTSEN] and CTSCSR0[NTASKS] by the end of boot,
do not set it inside simulator CFG files!
