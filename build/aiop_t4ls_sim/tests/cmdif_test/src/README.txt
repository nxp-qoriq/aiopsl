*******************************************************************************
README for cmdif_integ_dbg target
this is the latest demo
*******************************************************************************

===========================================
Build
===========================================
0. Use aiop sldevelop git and mc develop git
1. Build mc\build\mc_t4ls_sim\tests\cmdif_aiop target gpp_srv_dbg
2. Open aiopsl\src\arch\ppc\platform\ls2100a_aiop\cmdif.h and uncomment #define CMDIF_TEST_WITH_MC_SRV
3. Build aiopsl\build\aiop_t4ls_sim\tests\cmdif_test target cmdif_integ_dbg

===========================================
Setup
===========================================
1. Copy dpl.dtb from mc\tests\cmdif_gpp\dpl.dtb
2. Modify this line in simulator ls2100_sys_test.cfg file as follows:
   type=10 vc=25 addr_h=0x0 addr_l=0x1f800000 mem_space=0x10 cmdif_integ_dbg.elf  #for elf loader
3. Copy aiopsl\build\aiop_t4ls_sim\tests\cmdif_test\integ_out\cmdif_integ_dbg.elf
   into simulator folder

===========================================
Execution flow
===========================================
1. Run mc\build\mc_t4ls_sim\tests\cmdif_aiop gpp_srv_dbg.launch
2. Wait untill you'll see "MC is in busy waiting..."
3. Pause move to line right after while(1){} and push Run.
4. You'll see a lot of debug prints.
5. At the end you should see "Massive TEST PASSED!!!!" + "MC is in busy waiting..."
   and MC is again in the while(1) {} loop.






*******************************************************************************
README for cmdif_dbg target only for AIOP_STANDALONE single core mode
don't run it, it's an old demo
*******************************************************************************

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
