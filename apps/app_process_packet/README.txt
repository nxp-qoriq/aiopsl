
===========================================
Introduction
===========================================
This file contains instructions for running the app_process_packet demo.
This demo shows an "autonomous" packet flow through the AIOP and illustrates the following:
1. MC initialization
2. MC DP object creation from a layout file
3. AIOP initialization
4. AIOP object discovery
5. Packet injection (from a PCAP file)
6. Packet modification within the AIOP - reflection (Flip the MAC and IP source and destination addresses)
7. Packet transmission from the AIOP (to a PCAP file)

The app_process_packet project is intended to demonstrate how to write applications for the AIOP, through a very simple example.
It is not meant to demonstrate AIOP performance.

===========================================
LS2 HW Setup
===========================================
1. Install Code Warrior (see Release Note for the compatible CW version).
2. Import the aiopsl/build/aiop_sim/apps/app_process_packet/.project into CodeWarrior and build aiop_app.elf.    
2. Use LS2085QDS or LS2085RDB default setup.
   Connect UART ports to terminals. 
   AIOP and MC UART ports are configurable by dpc.dts.
3. Turn ON the board and get to the UBOOT prompt ==> on vBank 0.
4. Copy the following files into your TFTP folder:
   - aiopsl/misc/setup/dpl.dtb 
   - aiopsl/misc/setup/dpc.dtb
   - aiopsl/build/aiop_sim/apps/app_process_packet/out/aiop_app.elf
   - mc.itb
   - PBL.bin
   - u-boot.bin

===========================================
LS2 HW Execution flow
===========================================
1. Burn all the images to vBank 4 (you should be on vBank 0 while doing it)
   setenv ipaddr 192.168.1.4;setenv serverip 192.168.1.1;setenv netmask 255.255.0.0; ping $serverip;
   setenv filesize; setenv myaddr 0x584000000; tftp 0x80000000 /<path to your folder>/PBL.bin; protect off $myaddr +$filesize; erase $myaddr +$filesize; cp.b 0x80000000 $myaddr $filesize; protect on $myaddr +$filesize
   setenv filesize; setenv myaddr 0x584100000; tftp 0x80000000 /<path to your folder>/u-boot.bin; protect off $myaddr +$filesize; erase $myaddr +$filesize; cp.b 0x80000000 $myaddr $filesize; protect on $myaddr +$filesize
   setenv filesize; setenv myaddr 0x584300000; tftp 0x80000000 /<path to your folder>/mc.itb; protect off $myaddr +$filesize; erase $myaddr +$filesize; cp.b 0x80000000 $myaddr $filesize; protect on $myaddr +$filesize
   setenv filesize; setenv myaddr 0x584700000; tftp 0x80000000 /<path to your folder>/dpl.dtb; protect off $myaddr +$filesize; erase $myaddr +$filesize; cp.b 0x80000000 $myaddr $filesize; protect on $myaddr +$filesize
   setenv filesize; setenv myaddr 0x584800000; tftp 0x80000000 /<path to your folder>/dpc.dtb; protect off $myaddr +$filesize; erase $myaddr +$filesize; cp.b 0x80000000 $myaddr $filesize; protect on $myaddr +$filesize
   setenv filesize; setenv myaddr 0x584900000; tftp 0x80000000 /<path to your folder>/aiop_app.elf; protect off $myaddr +$filesize; erase $myaddr +$filesize; cp.b 0x80000000 $myaddr $filesize; protect on $myaddr +$filesize
2. Switch to vBank 4 and reboot.
   qixis_reset altbank
3. Look at the UART-GPP console and see: "fsl-mc: Management Complex booted"
4. Look at the UART-AIOP console and see: "AIOP boot finished; ready for tasks..."
5. Inject packets using aiopsl/misc/setup/eth_ipv4_udp.pcap
6. Look at the UART-GPP console and see that the packets've arrived.

===========================================
Simultor Setup
===========================================
1. Install Code Warrior (see Release Note for the compatible CW version).
2. Download the linux version of the simulator (see Release Note for the compatible LS_SIM version).
3. 1. For LS2085a simulator, copy the files ls2085a_sim_init_params.cfg and ls2085a_sys_test.cfg
      from the source tree at: aiopsl/misc/sim/.
      into the simulator folder at: dtsim_release/linux64/
3. 2. For LS1088a, copy the files ls1088a_sim_init_params.cfg and ls1088a_sys_test.cfg
      from the source tree at: aiopsl/misc/sim/.
      into the simulator folder at: dtsim_release/linux64/
4. Update the “LD_LIBRARY_PATH” variable to point to the simulator folder.
   setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/home/user/LS_SIM_<version>/dtsim_release/linux64
5. Copy the layout file (dpl.dtb) from aiopsl/misc/setup/ to the simulator folder.
6. Copy the configuration file (dpc.dtb) from aiopsl/misc/setup/ to the simulator folder.
7. Copy “eth_ipv4_udp.pcap” from the source tree at: aiopsl/misc/setup/ into the simulator folder

===========================================
Simulator Execution flow
===========================================
1. Import the MC and AIOP projects into CodeWarrior:
   mc/build/mc_sim/mc_app/.project
   aiopsl/build/aiop_sim/apps/app_process_packet/.project
2. Copy the resulting ELF file from the build project folder(aiop_app.elf)
   to the simulator folder (same location as cfg files).
3. 1. For LS2085a:
      Run the simulator:
      ./ccssim2 -port 42333 -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg" -smodel "ls_sim_config_file=ls2085a_sys_test.cfg"   
3. 2. For LS1088a:
      Run the simulator:
      ./ccssim2 -d ls1088aiss -port 42333 -imodel "ls_sim_init_file=ls1088a_sim_init_params.cfg" -smodel "ls_sim_config_file=ls1088a_sys_test.cfg"  
4. Launch mc_app using AFM connection.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
5. Attach app_process_packet (make sure to un-mark initialization files).
6. After MC reaches main(), run tio console:
   ./bin/tio_console -hub localhost:42975 -ser duart2_1 duart2_0
7. Run mc_app.
8. Run “tio capture”:
   ./fm_tio_capture -hub localhost:42975 -ser w0_m1 -verbose_level 2
   Here you'll be able to capture sent and received packets.
9 Run “tio inject”:
   ./fm_tio_inject -hub localhost:42975 -ser w0_m1 -file eth_ipv4_udp.pcap -verbose_level 2
   This will send packets to the AIOP.
10 Set break point inside app_process_packet() and push "Resume / Multi core Resume" button to run and see that
    it's activated on each packet.
11 The packet will also be captured by the tio_capture

===========================================
Possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. The user may add packet processing code inside app_process_packet()
3. The user may use different tio port and update it inside ls2085a_sys_test.cfg
4. The user may use different simulator port
5. The demo runs in MC integrated mode. MC loads AIOP and kicks the AIOP cores.
   Please note that the standalone mode is being phased out and is no longer verified.


