===========================================
Introduction
===========================================
The following file includes the instructions for the CAPWAP/DTLS demo
(cwap_dtls_demo).
Currently, the demo works only on LS2085A and LS2088A boards.

For (generic) instructions on how to write the binaries in flash etc.,
please refer to app_process_packet application README.

===========================================
Functional Description
===========================================
The following operations are performed during initialization:
- a CAPWAP/DTLS instance is created
- an Outbound DTLS SA (Security Association) is created
- an Inbound DTLS SA is created

The following operations are performed when a packet is injected:
- the original frame header is saved
- the original frame is sent to encryption, creating a new encrypted frame
- the encrypted frame is sent to decryption, creating a new decrypted frame
- the original frame is compared to the decrypted frame
- the decrypted frame is sent out

In other words, there is an "internal loopback" at application level,
CAPWAP/DTLS-encrypted frames from the Outbound DTLS SA being sent for
CAPWAP/DTLS-decryption to the Inbound DTLS SA.

See the section "Console Output Reference" at the end of this file
for sample output.

===========================================
Setup
===========================================
In order to run this test, follow the instructions at
"aiopsl/build/aiop_sim/apps/app_process_packet/src/README.txt",
while using the sample pcap files from "aiopsl/misc/setup/traffic_files/capwap*"
for traffic injection.
Note that the pcap files contain the 4-byte FCS, which needs to be dropped
before frames being injected (for e.g. using "tcprewrite --efcs").

=========================
Console Output Reference
=========================
This is an example of an output printed on the AIOP console:

[...]
CAPWAP/DTLS Demo: running app_init()...
CAPWAP/DTLS Demo: initialization will occur when the first frame is received
CAPWAP/DTLS Demo: sample pcap files can be found in: "aiopsl/misc/setup/traffic_files/capwap*"
[...]
CAPWAP/DTLS Demo: ENTERED app_process_packet
++++
  CAPWPAP/DTLS Demo: Doing CAPWAP/DTLS Initialization...
++++
CAPWAP/DTLS Demo: New Buffer Mode
CAPWPAP/DTLS Demo: SPID = 5
slab_create() completed successfully
slab_acquire() completed successfully
slab_acquire() completed successfully
Auth key addr = 0x60_9ab180
cwap_dtls_create_instance() completed successfully
cwap_dtls_add_sa_descriptor(encryption) succeeded
Encryption handle = 0x60_9a9180
cwap_dtls_add_sa_descriptor(decryption) succeeded
Decryption handle = 0x60_9a8180
CAPWAP/DTLS Demo: CAPWAP/DTLS initialization completed
CAPWAP/DTLS Demo: Core 15 Received Frame number 1
Encryption handle = 0x60_9a9180
Decryption handle = 0x60_9a8180
CAPWAP/DTLS Demo: Full frame before encryption : len = 114 bytes
0000: 00 00 01 00 00 01 00 85 11 ab c8 95 08 00 45 00
0010: 00 64 00 01 00 00 ec 11 91 2d ad 79 3f 9f 73 bc
0020: dc 85 0c ae 14 7e 00 50 98 5a 00 10 e2 80 00 00
0030: 00 00 00 00 00 15 00 02 ab 00 00 0d 02 a4 03 01
0040: 02 a0 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a
0050: 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a
0060: 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a
0070: 1a 1a
CAPWAP/DTLS Demo: Starting Encryption
cwap_dtls_frame_encrypt() completed successfully
CAPWAP/DTLS Demo: Full frame after encryption : len = 155 bytes
0000: 00 00 01 00 00 01 00 85 11 ab c8 95 08 00 45 00
0010: 00 8d 00 01 00 00 ec 11 91 04 ad 79 3f 9f 73 bc
0020: dc 85 0c ae 14 7e 00 79 98 5a 01 00 00 00 17 fd
0030: fe 01 23 45 67 89 ab cd ef 00 58 92 b0 70 6f 35
0040: be d5 e9 6e a3 0b 8a 6a 07 d0 b3 f8 48 6d ad ea
0050: 5b da 61 fe eb e8 1f d4 34 f2 f0 22 e2 00 f7 d4
0060: 21 d0 af 22 0b be ba be 85 4d ac 31 eb c1 20 90
0070: 31 9d 00 43 f0 06 c7 22 1c a9 eb ce 0e 84 f2 c0
0080: 9b 46 05 5d 4b 1a e3 25 13 b9 83 d9 0e c6 0a 35
0090: 16 0f 9a 90 d5 86 30 27 0c 17 a8
CAPWAP/DTLS Demo: Starting Decryption
cwap_dtls_frame_decrypt() completed successfully
STATUS: fdma_present_default_frame_segment returned 0
CAPWAP/DTLS Demo: Full frame after decryption : len = 114 bytes
0000: 00 00 01 00 00 01 00 85 11 ab c8 95 08 00 45 00
0010: 00 64 00 01 00 00 ec 11 91 2d ad 79 3f 9f 73 bc
0020: dc 85 0c ae 14 7e 00 50 98 5a 00 10 e2 80 00 00
0030: 00 00 00 00 00 15 00 02 ab 00 00 0d 02 a4 03 01
0040: 02 a0 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a
0050: 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a
0060: 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a 1a
0070: 1a 1a
CAPWAP/DTLS Demo: statistics for encryption:
epoch[63:48], sequence_number[47:0] = 0x123456789abcdf0
CAPWAP/DTLS Demo: statistics for decryption:
epoch[63:48], sequence_number[47:0] = 0x123456789abcdef
anti-replay bitmap[0:3] = 0x1, 0x0, 0x0, 0x0
CAPWAP/DTLS Demo: Core 15 Sending Frame number 1
Finished SUCCESSFULLY
Frame after decryption the same as origin
CAPWAP/DTLS Demo: Done Sending Frame
