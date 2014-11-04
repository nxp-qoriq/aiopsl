===========================================
Introduction
===========================================
This file contains instructions for running the ip fragmentation demo.
This demo shows packet fragmentation done by the AIOP and illustrates the following:
1. MC initialization
2. MC DP object creation from a layout file
3. AIOP initialization
4. AIOP object discovery
5. Packet injection (from a PCAP file)
6. Packet fragmentation (and modification) within the AIOP
7. Packet transmission from the AIOP (to a PCAP file)

===========================================
Setup
===========================================
1. Install Code Warrior (see Release Note for the compatible CW version).
2. Download the linux version of the simulator (see Release Note for the compatible LS_SIM version).
3. Copy the files ls2085a_sim_init_params.cfg and ls2085a_sys_test.cfg
   from the source tree at: aiopsl/build/aiop_sim/sim_files. 
   into the simulator folder at: dtsim_release/linux64/
   comment the lines marked with "#for elf loader".
   (you may need to change "localhost" to your Linux machine name).
4. Update the “LD_LIBRARY_PATH” variable to point to simulator folder.
  setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/home/user/LS_SIM_<version>/dtsim_release/linux64
5. Copy the layout file (dpl.dtb) from aiopsl/misc/setup/ to the simulator folder.
6. Copy following pcap files from aiopsl\misc\setup into to simulator folder:
 	reassembled_frame.pcap
 	frag1.pcap
 	frag2.pcap
 	frag3.pcap
 	frag4.pcap
 	frag1_hm.pcap
 	frag2_hm.pcap
 	frag3_hm.pcap
 	frag4_hm.pcap
 7. Set ipf_demo_flags in ipf_demo.c either to IPF_DEMO_WITH_HM or IPF_DEMO_WITHOUT_HM. 

===========================================
Execution flow
===========================================
1. Import the MC and AIOP projects into CodeWarrior:
   mc/build/mc_sim/mc_app/.project
   aiopsl/build/aiop_sim/apps/ipf_demo/.project
2. Build both projects in CW.
3. Copy the resulting ELF file from the build project folder(aiop_app.elf)
   to the simulator folder (same location as cfg files).
4. Run simulator:
   ./ccssim2 -port 42333
             -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg"
             -smodel "ls_sim_config_file=ls2085a_sys_test.cfg"
5. Launch mc_app using AFM connection.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
6. Attach ipf_demo (make sure to un-mark initialization files).
7. After MC reaches main(), turn tio console:
   ./bin/tio_console -hub localhost:42975 -ser duart2_1 duart2_0
8. Run mc_app.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
9. Run “tio capture”:
   ./fm_tio_capture -hub localhost:42975 -ser w0_m1 -verbose_level 2
   Here you'll be able to capture sent and received packets.
10. Run “tio inject” to inject the input frame (reassembled_frame.pcap):
   ./fm_tio_inject -hub localhost:42975 -ser w0_m1 -file reassembled_frame.pcap -verbose_level 2
   This will send a packet to AIOP.
11. Set break point inside app_process_packet_flow0() and push "Multi core Resume" button to run and see that
    it's activated on each packet.
12. The 4 output fragments will also be captured by the tio_capture.
13. In the simulator directory you will get a pcap output file: w0_m1.pcap.
	This file should include the 4 output fragments, which must be identical to:
	frag1.pcap, frag2.pcap, frag3.pcap, frag4.pcap in case you run with IPF_DEMO_WITHOUT_HM
	or to
	frag1_hm.pcap, frag2_hm.pcap, frag3_hm.pcap, frag4_hm.pcap in case you run with IPF_DEMO_WITH_HM.	

===========================================
Possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. The user may add packet processing code inside app_process_packet_flow0()
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
  Parser Profile_id = 0 - only parser profile id 0 is supported
  MTU = maximal value

