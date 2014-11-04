*******************************************************************************
README for cmdif_integ_dbg target
*******************************************************************************

===========================================
Build
===========================================
0. Use aiop sldevelop git and mc develop git.
   Make sure that cmdif_client_init() is part of MC init.
   {{cmdif_client_init, cmdif_client_free}, MC_CMDIF_INIT_ERR},
1. Build mc\build\mc_t4ls_sim\tests\cmdif_aiop target gpp_srv_dbg
2. Open aiopsl\src\arch\ppc\platform\ls2100a_aiop\cmdif.h and uncomment #define CMDIF_TEST_WITH_MC_SRV.
   This is required in order to test AIOP <-> MC.
3. Build aiopsl\build\aiop_sim\tests\cmdif_test target cmdif_integ_dbg

===========================================
Setup
===========================================
1. Copy dpl.dtb from aiopsl/misc/setup/dpl.dtb
2. Modify this line in simulator ls2100_sys_test.cfg file as follows:
   type=10 vc=25 addr_h=0x0 addr_l=0x1f800000 mem_space=0x10 cmdif_integ_dbg.elf  #for elf loader
3. Copy aiopsl\build\aiop_sim\tests\cmdif_test\integ_out\cmdif_integ_dbg.elf
   into simulator folder

===========================================
Execution flow
===========================================
1. Run mc\build\mc_t4ls_sim\tests\cmdif_aiop gpp_srv_dbg.launch
2. You'll see a lot of debug prints.
3. At the end you should see "Massive TEST PASSED!!!!" + "MC is in busy waiting..." and MC is again in the while(1) {} loop.

