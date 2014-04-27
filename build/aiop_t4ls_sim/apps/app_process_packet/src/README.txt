The following file includes the instructions for app_process_packet demo.

Use ViPR to setup the following settings. 
Please refer to examples in TLS simulator installation folder at LS2100-AFM_SIM_ALPHA1\demo_ls\aiop\
to initialize all the AIOP accelerators. 
Use AIOP standalone mode.

The aiop_packet_processing demo assumes the following additional settings.

Set first EPID table entry with the following settings:
===========================================
1. EP_PM = NI id that you've registered to using dpni_drv_register_rx_cb.

FD[FLC] - frame descriptor special settings
===========================================
1. Set EPID to 0, EPID entry 0 will include the NI id under EP_PM.  
2. Set APPIDX to the flow id number that you've registered to using dpni_drv_register_rx_cb.

===========================================
Demo possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()
2. The user may add packet processing code inside app_process_packet()

===========================================
Execution flow
===========================================
1. Run the demo and get till the core is at waiting state
2. Start enqueueing FDs using ViPR environment shell.
3. See that app_process_packet() is called on every FD.
 
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