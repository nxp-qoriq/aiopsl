===========================================
Introduction
===========================================
This file contains instructions for running the CAPWAP fragmentation demo.

This demo shows fragmentation of CAPWAP frames done by the AIOP.
In order to run this test follow the instructions at "aiopsl/build/aiop_sim/apps/app_process_packet/src/README.txt".

Use aiopsl/misc/setup/capwap_control.pcap instead of 
    aiopsl/misc/setup/eth_ipv4_udp.pcap in order to inject the packets.

In the simulator directory you will get a pcap output file: w0_m1.pcap.
This file should include the output fragments.   
