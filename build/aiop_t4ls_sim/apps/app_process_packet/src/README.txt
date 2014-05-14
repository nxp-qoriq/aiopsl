The following file includes the instructions for app_process_packet demo.

===========================================
Setup
===========================================
1. Install CW_DPAA_v10.0.7
2. Download linux version of DPAA_SIM_RELEASE_0_7_0_0101 and overwrite
   libctlufunc.so.1, libparserfunc.so.0 files by files from the following patch:
   http://gforge.freescale.net/frs/download.php/2041/LS2100_SIM_CTLU-20140418.tbz2
3. Copy into simulator folder ls2085a_sim_init_params.cfg , ls2100_sys_test.cfg
   from aiopsl\build\aiop_t4ls_sim\sim_files.
4. Copy the dpl.dtb file from aiopsl\misc\setup to simulator folder.
5. Copy “eth_ipv4_udp.pcap” from aiopsl\misc\setup into to simulator folder

===========================================
Execution flow
===========================================
1. Build mc_app from mc_release_0.3.2 using CW_DPAA_v10.0.7.
2. Build app_process_packet from aiopsl using CW_DPAA_v10.0.7.
3. Run simulator:
   ./ccssim2 -port 42333
             -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg"
             -smodel "ls_sim_config_file=ls2100_sys_test.cfg"
4. Launch mc_app using AFM connection.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
5. After MC reaches main(), tun tio console:
   ./bin/tio_console -hub localhost:42975 -ser duart1_1 duart1_0
6. Run mc_app untill you'll see "Running MC app, waiting for events ..." on tio console
7. Launch app_process_packect.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
8. Push "Multicore Resume" button to run all AIOP cores untill you'll see
   "AIOP <core_id> completed boot sequence; waiting for events ..." for all cores
9. Run “tio capture”:
   ./fm_tio_capture -hub localhost:42975 -ser w0_m1 -verbose_level 2
   Here you'll be able to capture sent and received packets.
10. Run “tio inject”:
   ./fm_tio_inject -hub localhost:42975 -ser w0_m1 -file eth_ipv4_udp.pcap -verbose_level 2
   This will send packets to AIOP.
11. Set break point inside app_process_packet_flow0() and push "Multicore Resume" button to run and see that
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
   aiopsl and app_process_packect project with AIOP_STANDALONE defined.
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

