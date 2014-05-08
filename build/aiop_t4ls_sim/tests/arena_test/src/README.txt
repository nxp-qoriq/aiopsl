
===========================================
Demo possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. The user may add packet processing code inside app_process_packet_flow0()

===========================================
Execution flow
===========================================
1. Run the demo and get till the core is at waiting state
2. Start enqueueing FDs using ViPR environment shell.
3. See that app_process_packet_flow0() is called on every FD.
 
=================
Important NOTEs:
=================
ARENA SW sets CTSCSR0[CTSEN] and CTSCSR0[NTASKS] by the end of boot, 
do not set it inside ViPR 

ARENA sets the following default values for every NI: 
===========================================
  profile_id = 0 - only parser profile id 0 is supported
  QDID = 0 - Queueing destination for the enqueue
  spid = 0 - Storage profile ID
  MTU = maximal value

BMAN pools
===========================================
Please note that BMAN pool ID 1 is used by parser initialization routine. 

Virtual pools
===========================================
Don't use virtual pools API, use SLAB API as demonstrated inside this demo

SLAB pools  
===========================================
Don't create your SLABs inside FD task, prepare them beforehand inside app_init() as demonstrated inside this demo