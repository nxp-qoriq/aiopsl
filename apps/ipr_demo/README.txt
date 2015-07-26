===========================================
Introduction
===========================================
This file contains instructions for running the IPR demo test.
This test injects 4 fragments and verify that the reassembled frame is correct.
In order to run this test follow the instructions at "aiopsl/build/aiop_sim/apps/app_process_packet/src/README.txt".
Use 4 fragments starting from aiopsl/misc/setup/frag1.pcap till aiopsl/misc/setup/frag4.pcap instead of 
    aiopsl/misc/setup/eth_ipv4_udp.pcap in order to inject the packets.

In the simulator directory you will get a pcap output file: w0_m1.pcap.
This file should include the reassembled frame, which must be identical to reassembled_frame_hm.pcap.
    