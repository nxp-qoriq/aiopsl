The following file includes the instructions for app_process_packet demo.

Set EPID table with the following settings:
===========================================
1. EP_PC = 0x00820000.
2. EPID = 0
3. EP_PM = 0 - only 1 NI is currently tested

FD[FLC] - frame descriptor special settings
===========================================
1. Set flow ID to 0

ARENA sets the following default values for every NI: 
===========================================
  profile_id = 0 - only parser profile id 0 is supported
  QDID = 0 - Queueing destination for the enqueue
  spid = 0 - Storage profile ID
  MTU = maximal value

Important NOTEs:
=================
ARENA SW sets CTSCSR0[CTSEN] and and CTSCSR0[NTASKS] by the end of boot, 
do not set it inside ViPR 

 