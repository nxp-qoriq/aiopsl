The following file includes the instructions for app_process_packet demo.

===========================================
Setup
===========================================
1. Install CW_APP_v10.0.8
2. Download linux version of LS_SIM_RELEASE_0_8_0_0112
3. Copy into simulator folder ls2085a_sim_init_params.cfg , ls2100_sys_test.cfg
   from aiopsl\build\aiop_t4ls_sim\sim_files.
4. Update the “LD_LIBRARY_PATH” variable to point to simulator folder.
   setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/home/user/DPAA_SIM_RELEASE_0_7_0_0101/dtsim_release/linux64
5. Copy the dpl.dtb file from aiopsl\misc\setup to simulator folder.
6. Copy “eth_ipv4_udp.pcap” from aiopsl\misc\setup into to simulator folder

===========================================
Execution flow
===========================================
1. Build mc_app from mc_release_0.4.1 using CW_APP_v10.0.8.
2. Build app_process_packet from aiopsl using CW_APP_v10.0.8.
3. Run simulator:
   ./ccssim2 -port 42333
             -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg"
             -smodel "ls_sim_config_file=ls2100_sys_test.cfg"
4. Launch mc_app using AFM connection.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
5. After MC reaches main(), turn tio console:
   ./bin/tio_console -hub localhost:42975 -ser duart1_1 duart1_0
6. Run mc_app until you'll see "Running MC app, waiting for events ..." on tio console
7. Launch app_process_packet.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
8. Push "Resume" button to run Single AIOP core and wait until you'll see
   "AIOP <0> completed boot sequence; waiting for events ..." 
   (for using multi core mode define in preprocessor MULTICORE_WA   [inside CW Project -> Properties->
	C/C++ Build -> Settings: Preprocessor -> Defined Macros]  and use "multi-core resume" when starting debugger)
9. Run “tio capture”:
   ./fm_tio_capture -hub localhost:42975 -ser w0_m1 -verbose_level 2
   Here you'll be able to capture sent and received packets.
10. Run “tio inject”:
   ./fm_tio_inject -hub localhost:42975 -ser w0_m1 -file eth_ipv4_udp.pcap -verbose_level 2
   This will send packets to AIOP.
11. Set break point inside app_process_packet_flow0() and push "Resume / Multi core Resume" button to run and see that
    it's activated on each packet.
12. The packet will also be captured by the tio_capture

===========================================
Possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. The user may add packet processing code inside app_process_packet()
3. The user may use different tio port and update it inside ls2100_sys_test.cfg
4. The user may use different simulator port
5. The demo runs in MC integrated mode. In order to get back to AIOP standalone
   mode as it was supported in previous releases, it is required to recompile
   aiopsl and app_process_packet project with AIOP_STANDALONE defined.
   Please note that the standalone mode is being phased out and has is no longer verified.
6. The user may run with elf loader. In order to do that:
   a.	Go to simulator folder and uncomment the lines marked with "#for elf loader" in cfg
		files: "ls2100_sys_test", "ls2085a_sim_init_params"
   b.	Copy the "aiop_app.elf" from the build project folder to the simulator folder in same location
		as the cfg files.
   c.   For interactive debugging of aiop code, run debug configurations from CW after you'll see
		"AIOP <0> completed boot sequence; waiting for events ..."
		Choose the relevant project (app_process_packet) and switch in debug session type to "Attach".
		in target settings choose AIOP-2 as target and choose edit.
		(verify that the server port and host name are the same as in simulator). enter edit..., 
		and un-mark initialization files.
		

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

