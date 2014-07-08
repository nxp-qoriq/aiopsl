The following file includes the instructions for ipr_demo.

===========================================
Setup
===========================================
1. Install CW_APP_v10.0.8
2. Download linux version of LS_SIM_RELEASE_0_8_0_0116
3. Copy into simulator folder ls2085a_sim_init_params.cfg , ls2085a_sys_test.cfg
   from aiopsl\build\aiop_t4ls_sim\sim_files.
   comment the lines marked with "#for elf loader" in cfg files: 
   "ls2100_sys_test" and "ls2085a_sim_init_params"
   (you may need to change "localhost" to your Linux machine name).
4. Update the “LD_LIBRARY_PATH” variable to point to simulator folder.
   setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/home/user/DPAA_SIM_RELEASE_0_8_0_0116/dtsim_release/linux64
5. Copy the dpl.dtb file from aiopsl\misc\setup to simulator folder.
6. Copy “reassembled_frame.pcap" from aiopsl\misc\setup into to simulator folder

===========================================
Execution flow
===========================================
1. Build mc_app from mc_release_0.4.2 using CW_APP_v10.0.8.
2. Build ipf_demo from aiopsl using CW_APP_v10.0.8.
3. Run simulator:
   ./ccssim2 -port 42333
             -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg"
             -smodel "ls_sim_config_file=ls2085a_sys_test.cfg"
4. Launch mc_app using AFM connection.
	Init file: mc\build\mc_t4ls_sim\mc_app\CFG\LS2085A-AFM_MC_RAM.tcl.
	Mem file: mc\build\mc_t4ls_sim\mc_app\CFG\LS2085A-AFM_MC.mem
   Don't forget to update simulator server IP and port in debug configuration.
5. After MC reaches main(), turn tio console:
   ./bin/tio_console -hub localhost:42975 -ser duart1_1 duart1_0
6. Run mc_app.
7. Launch ipf_demo using AFM connection.
	Init file: aiopsl\build\aiop_t4ls_sim\cw_files\LS2085A-AFM_AIOP_RAM
	Mem file: aiopsl\build\aiop_t4ls_sim\cw_files\LS2085A-AFM_AIOP.mem
   Don't forget to update simulator server IP and port in debug configuration. 
8. Run aiop (multi-core resume).
9. Run “tio capture”:
   ./fm_tio_capture -hub localhost:42975 -ser w0_m1 -verbose_level 2
   Here you'll be able to capture sent and received packets.
10. Run “tio inject” to inject all 4 fragments:
   ./fm_tio_inject -hub localhost:42975 -ser w0_m1 -file reassembled_frame.pcap -verbose_level 2
   This will send packets to AIOP.
11. Set break point inside app_process_packet_flow0() and push "Multi core Resume" button to run and see that
    it's activated on each packet.
12. The packet will also be captured by the tio_capture

===========================================
Possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. The user may add packet processing code inside app_process_packet()
3. The user may use different tio port and update it inside ls2085a_sys_test.cfg
4. The user may use different simulator port
5. The demo runs in MC integrated mode. In order to get back to AIOP standalone
   mode as it was supported in previous releases, it is required to recompile
   aiopsl and app_process_packet project with AIOP_STANDALONE defined.
   Please note that the standalone mode is being phased out and has is no longer verified. 

=================
Important NOTEs:
=================
ARENA SW sets CTSCSR0[CTSEN] and CTSCSR0[NTASKS] by the end of boot,
do not set it inside simulator cfg files

===========================================
ARENA sets the following default values for every NI:
===========================================
  profile_id = 0 - only parser profile id 0 is supported
  QDID = 0 - Queueing destination for the enqueue
  spid = 0 - Storage profile ID
  MTU = maximal value

==========
ARENA APIs
==========
Don't use virtual pools API, use SLAB API as demonstrated inside this demo
For spinlock, use only: lock_spinlock and unlock_spinlock
All other spinlock functions will be removed in future release.

