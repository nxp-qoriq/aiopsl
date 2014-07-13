The following file includes the instructions for the IPsec Functional Module demo (ipsec_fm_demo).

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
1. Install CW_APP_v10.0.8
2. Download linux version of LS_SIM_m0116
3. Copy into simulator folder ls2085a_sim_init_params.cfg , ls2100_sys_test.cfg
   from aiopsl\build\aiop_t4ls_sim\sim_files.
4. Update the “LD_LIBRARY_PATH” variable to point to simulator folder.
   setenv LD_LIBRARY_PATH {$LD_LIBRARY_PATH}:/home/user/LS_SIM_m0116/dtsim_release/linux64
5. Copy the dpl.dtb file from aiopsl\misc\setup to simulator folder.
6. Copy “eth_ipv4_udp.pcap” from aiopsl\misc\setup into to simulator folder

===========================================
Execution flow
===========================================
1. Build mc_app using CW_APP_v10.0.8.
2. Build ipsec_fm_demofrom aiopsl using CW_APP_v10.0.8.
3. Copy resulting ELF file from the build project folder(aiop_app.elf) to simulator folder (same location as cfg files).
4. Run simulator:
   ./ccssim2 -port 42333
             -imodel "ls_sim_init_file=ls2085a_sim_init_params.cfg"
             -smodel "ls_sim_config_file=ls2100_sys_test.cfg"
5. Launch mc_app using AFM connection.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
6. Attach ipsec_fm_demo(make sure to un-mark initialization files).
7. After MC reaches main(), turn tio console:
   ./bin/tio_console -hub localhost:42975 -ser duart1_1 duart1_0
8. Run mc_app.
   Don't forget to update simulator server IP and port in debug configuration - 42333.
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
   aiopsl and ipsec_fm_demoproject with AIOP_STANDALONE defined.
   Please note that the standalone mode is being phased out and has is no longer verified.
6. The user may run without elf loader. In order to do that go to simulator folder and
   comment the lines marked with "#for elf loader" in cfg files:
   "ls2100_sys_test" and "ls2085a_sim_init_params"

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
  MTU = maximal value

==========
ARENA APIs
==========
Don't use virtual pools API, use SLAB API as demonstrated inside this demo.


=========================
TIO Output Reference
=========================
This is an example of an output printed on the TIO console:


++++
  IPsec Demo: Doing IPsec Initialization
+++
> TRACE [CPU00, cmdif_srv.c:429 mc_portal_isr]: got cmd @portal1
ipsec_create_instance() completed successfully
slab_create() completed successfully
slab_acquire() completed successfully
slab_acquire() completed successfully
ipsec_add_sa_descriptor(encryption) succeeded
Encryption handle = 0x0_5834efa0
ipsec_add_sa_descriptor(decryption) succeeded
Decryption handle = 0x0_5834ed60
IPsec Demo: IPsec Initialization completed
> INFO [CPU 0, init.c:167 core_ready_for_tasks]: AIOP core 0 completed boot sequence
> INFO [CPU 0, init.c:172 core_ready_for_tasks]: AIOP boot finished; ready for tasks...
IPsec Demo: Core 0 Received Frame
Encryption handle = 0x0_5834efa0
Decryption handle = 0x0_5834ed60
IPSEC: frame header before encryption
00  00 00 01 00 00 01 00 10  94 00 00 02 08 00 45 00
10  00 6e 00 00 00 00 ff 11  3a 26 c0 55 01 02 c0 00
20  00 01 04 00 04 00 00 5a  ff ff 00 00 00 00 00 00
30  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
40  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
50  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
60  00 00 00 00 00 00 00 00  f5 d0 64 51 ac 9f 69 d4
70  d3 f7 39 6e 20 0e 97 b7  e9 e4 56 3a

IPSEC: Starting Encryption
ipsec_frame_encrypt() completed successfully
IPSEC: frame header after encryption
00  00 00 01 00 00 01 00 10  94 00 00 02 08 00 45 db
10  00 98 12 34 00 00 ff 32  37 eb 45 a4 e1 4c ed 03
20  5c 45 00 00 00 00 00 00  00 00 45 00 00 6e 00 00
30  00 00 ff 11 3a 26 c0 55  01 02 c0 00 00 01 04 00
40  04 00 00 5a ff ff 00 00  00 00 00 00 00 00 00 00
50  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
60  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
70  00 00 00 00 00 00 00 00  00 00 00 00

IPSEC: Starting Decryption
ipsec_frame_decrypt() completed successfully
IPSEC: frame header after decryption
00  00 00 01 00 00 01 00 10  94 00 00 02 08 00 45 00
10  00 6e 00 00 00 00 ff 11  3a 26 c0 55 01 02 c0 00
20  00 01 04 00 04 00 00 5a  ff ff 00 00 00 00 00 00
30  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
40  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
50  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
60  00 00 00 00 00 00 00 00  f5 d0 64 51 ac 9f 69 d4
70  d3 f7 39 6e 20 0e 97 b7  e9 e4 56 3a

SUCCESS: frame after decryption the same as origin
IPsec Demo: Core 0 Sending Frame
IPsec Demo: Done Sending Frame
