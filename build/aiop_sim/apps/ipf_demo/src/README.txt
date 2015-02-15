===========================================
Introduction
===========================================
This file contains instructions for running the ip fragmentation demo.

This demo shows packet fragmentation done by the AIOP, and verifies several modules like: fdma, parser etc.
In order to run this test follow the instructions at "aiopsl/build/aiop_sim/apps/app_process_packet/src/README.txt".
Use aiopsl/misc/setup/reassembled_frame.pcap instead of 
    aiopsl/misc/setup/eth_ipv4_udp.pcap in order to inject the packets.

In the simulator directory you will get a pcap output file: w0_m1.pcap.
This file should include the 4 output fragments, which must be identical to:
frag1_hm.pcap, frag2_hm.pcap, frag3_hm.pcap, frag4_hm.pcap.   
