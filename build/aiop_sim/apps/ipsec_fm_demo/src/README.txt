===========================================
Introduction
===========================================
The following file includes the instructions for the IPsec Functional Module demo (ipsec_fm_demo).
This demo shows an "autonomous" packet flow through the AIOP and illustrates the following:
1. MC initialization
2. MC DP object creation from a layout file
3. AIOP initialization
4. AIOP object discovery
5. Packet injection (from a PCAP file)
6. Packet encryption and decryption within the AIOP
7. Packet transmission from the AIOP (to a PCAP file)

===========================================
Functional Description
===========================================
This demo is doing the following operations during initialization:
- Creates an IPsec instance
- Creates an Outbound SA descriptor
- Creates an Inbound SA descriptor

This demo is doing the following operations when a packet is injected:
- Saves the original frame header
- Sending the original frame to encryption, creating a new encrypted frame
- Sending the encrypted frame to decryption, creating a new decrypted frame
- Comparing the original frame to the decrypted frame
- Sending the frame

* See the section "TIO Output Reference" at the end of this file.

===========================================
Setup
===========================================
1. Install Code Warrior (see Release Note for the compatible CW version).
2. Download the linux version of the simulator (see Release Note for the compatible LS_SIM version).
3. Copy the files ls2085a_sim_init_params.cfg and ls2085a_sys_test.cfg
   from the source tree at: aiopsl/build/aiop_sim/sim_files.
   into the simulator folder at: dtsim_release/linux64/
4. Update the “LD_LIBRARY_PATH” variable to point to the simulator folder.
   setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/home/user/LS_SIM_<version>/dtsim_release/linux64
5. Copy the layout file (dpl.dtb) from aiopsl/misc/setup/ to the simulator folder.
6. Copy “eth_ipv4_udp.pcap” from the source tree at: aiopsl/misc/setup/ into the simulator folder

===========================================
Execution flow
===========================================
1. Import the MC and AIOP projects into CodeWarrior:
   mc/build/mc_sim/mc_app/.project
   aiopsl/build/aiop_sim/apps/ipsec_fm_demo/.project
2. Build both projects in CW.
3. Copy the resulting ELF file from the build project folder(aiop_app.elf)
   to the simulator folder (same location as cfg files).
4. Run the simulator:
   ./ccssim2 -port 42333
             -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg"
             -smodel "ls_sim_config_file=ls2085a_sys_test.cfg"
5. Launch mc_app using AFM connection.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
6. Attach ipsec_fm_demo (make sure to un-mark initialization files).
7. After MC reaches main(), run tio console:
   ./bin/tio_console -hub localhost:42975 -ser duart2_1 duart2_0
8. Run mc_app.
9. Run “tio capture”:
   ./fm_tio_capture -hub localhost:42975 -ser w0_m1 -verbose_level 2
   Here you'll be able to capture sent and received packets.
10. Run “tio inject”:
   ./fm_tio_inject -hub localhost:42975 -ser w0_m1 -file eth_ipv4_udp.pcap -verbose_level 2
   This will send packets to the AIOP.
11. Set break point inside app_process_packet_flow0() and push "Resume / Multi core Resume" button to run and see that
    it's activated on each packet.
12. The packet will also be captured by the tio_capture

===========================================
Possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. The user may add packet processing code
3. The user may use different tio port and update it inside ls2085a_sys_test.cfg
4. The user may use different simulator port
5. The demo runs in MC integrated mode. MC loads AIOP and kicks the AIOP cores.
   Please note that the standalone mode is being phased out and is no longer verified.

===========================================
ARENA sets the following default values for every NI:
===========================================
MTU = maximal value
Same parser profile id for all NIs

=========================
TIO Output Reference
=========================
This is an example of an output printed on the TIO console:

Running app_init()
To start test inject packets: "eth_ipv4_udp.pcap"
(IPsec initialization will occur when the first frame is received)
> INFO [CPU 0, init.c:267 core_ready_for_tasks]: AIOP core 0 completed boot sequence
> INFO [CPU 0, init.c:273 core_ready_for_tasks]: AIOP boot finished; ready for tasks...

++++
  IPsec Demo: Doing IPsec Initialization
+++
IPsec Demo: SPID = 1
*** Debug: storage_profile (0): 0x9000c00
*** Debug: storage_profile (1): 0x0
*** Debug: storage_profile (2): 0x0
*** Debug: storage_profile (3): 0x200
*** Debug: storage_profile (4): 0x1080a80
*** Debug: storage_profile (5): 0x0
*** Debug: storage_profile (6): 0x0
*** Debug: storage_profile (7): 0x0
ipsec_create_instance() completed successfully
slab_create() completed successfully
slab_acquire() completed successfully
slab_acquire() completed successfully
Cipher Algorithm: IPSEC_CIPHER_AES_CBC
Authentication Algorithm: IPSEC_AUTH_HMAC_SHA2_256_128
ipsec_add_sa_descriptor(encryption) succeeded
Encryption handle = 0x60_db9208
ipsec_add_sa_descriptor(decryption) succeeded
Decryption handle = 0x60_db8e08
IPsec Demo: IPsec Initialization completed
IPsec Demo: Core 0 Received Frame number 1
Encryption handle = 0x60_db9208
Decryption handle = 0x60_db8e08
IPSEC: frame header before encryption
Printing Frame. FD[len] = 124, Seg Len = 124
0000  00 00 01 00 00 01 00 10  94 00 00 02 08 00 45 00
0010  00 6e 00 00 00 00 ff 11  3a 26 c0 55 01 02 c0 00
0020  00 01 04 00 04 00 00 5a  ff ff 00 00 00 00 00 00
0030  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
0040  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
0050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
0060  00 00 00 00 00 00 00 00  f5 d0 64 51 ac 9f 69 d4
0070  d3 f7 39 6e 20 0e 97 b7  e9 e4 56 3a

IPSEC: Starting Encryption
ipsec_frame_encrypt() completed successfully
IPSEC: frame header after encryption
Printing Frame. FD[len] = 206, Seg Len = 206
0000  00 00 01 00 00 01 00 10  94 00 00 02 86 dd 60 00
0010  00 00 00 98 32 ff fe 80  00 00 00 00 00 00 02 11
0020  25 ff fe 82 95 b5 ff 02  00 00 00 00 00 00 00 00
0030  00 01 ff 82 95 b5 00 00  00 00 00 00 00 00 00 00
0040  00 00 00 00 00 00 00 00  00 00 00 00 00 00 fc af
0050  94 50 a1 3c e8 59 35 f5  6c 5a 37 7d 8b b0 6a 5a
0060  3e 2f d0 23 6a 70 17 57  38 e4 03 a6 bf 43 2f f5
0070  31 99 81 b3 3d 16 e1 b2  d8 7e e9 ab 11 e3 c4 94
0080  d9 15 7d 27 a2 e6 5d 3e  cd 78 f7 75 d3 fd a1 06
0090  b1 8f c9 4c 9f 14 c7 7c  c7 d8 2f d5 85 d4 c3 63
00a0  49 0b 3b fa 13 92 29 f2  02 8e 9a 15 84 a5 b3 14
00b0  5c 02 5a ab af 48 25 9d  73 99 df fe 7b 64 66 5e
00c0  bb f9 51 f7 04 2f 27 c6  ef bd 21 3a e5 19

IPSEC: Starting Decryption
ipsec_frame_decrypt() completed successfully
IPSEC: frame header after decryption
Printing Frame. FD[len] = 124, Seg Len = 124
0000  00 00 01 00 00 01 00 10  94 00 00 02 08 00 45 00
0010  00 6e 00 00 00 00 ff 11  3a 26 c0 55 01 02 c0 00
0020  00 01 04 00 04 00 00 5a  ff ff 00 00 00 00 00 00
0030  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
0040  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
0050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
0060  00 00 00 00 00 00 00 00  f5 d0 64 51 ac 9f 69 d4
0070  d3 f7 39 6e 20 0e 97 b7  e9 e4 56 3a

Finished SUCCESSFULLY

Frame after decryption the same as origin

IPsec Demo: Encryption Statistics:
IPsec Demo: ipsec_get_lifetime_stats():
kilobytes = 0x0_0xb0,packets = 0x0_0x1, seconds = 0
IPsec Demo: ipsec_get_seq_num():
sequence_number = 0x1, esn = 0x0
bitmap[0:3] = 0x0, 0x0, 0x0, 0x0
IPsec Demo: Decryption Statistics:
IPsec Demo: ipsec_get_lifetime_stats():
kilobytes = 0x0_0x70,packets = 0x0_0x1, seconds = 0
IPsec Demo: ipsec_get_seq_num():
sequence_number = 0x0, esn = 0x0
bitmap[0:3] = 0x0, 0x0, 0x0, 0x0
IPsec Demo: Core 0 Sending Frame number 1
IPsec Demo: Done Sending Frame
