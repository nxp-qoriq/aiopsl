===========================================
Introduction
===========================================
This file contains instructions for running CWAPR demo application.

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
4. Look at the UART-AIOP console and see: CWAPR_DEMO:: Successfully finished initialization"
5. Inject packets available at aiopsl/misc/setup/:
	- capwap_frags_mtu100.pcap - 11 in-order fragments created with MTU of 100 bytes
	- capwap_frags_mtu128.pcap - 8 in-order fragments created with MTU of 128 bytes
	- capwap_frags_mtu300.pcap - 3 out-of-order fragments created with MTU of 300 bytes
	- capwap_frags_id5_1.pcap, capwap_frags_id5_2.pcap, capwap_frags_id5_3.pcap - 3 independent fragments
6. Look at the AIOP console and check if CWAPR statistics match injected traffic 	
7. Look at the UART-GPP console and see that the packets've arrived.    