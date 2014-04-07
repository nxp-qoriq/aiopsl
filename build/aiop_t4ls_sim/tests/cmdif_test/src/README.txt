The following file includes the instructions for cmdif_test demo.


Set EPID table entry with the following settings:
===========================================
1. EP_PC = 0x00fe0000.

FD[FLC] - frame descriptor special settings
===========================================
1. Set EPID entry index inside FD[FLC] bits 9-0.
2. Set FD[FLC] bits 31-16 as 0x2000, it will make FLC[checksum] = 0x2000

FD[BPID] - special settings
===========================================
1. Set FD[BPID] to 0x0

===========================================
Demo possible modifications:
===========================================
1. The user may add application additional initialization inside app_init()

===========================================
Execution flow
===========================================
1. Run the demo and get till the core is at waiting state
2. Start enqueueing FDs with FD[FLC] set as described above.
3. See that app_receive_cb() is called on every FD and the FD is sent back
   according to Frame Queue Context as received from QMan.

=================
Important NOTEs:
=================
ARENA SW sets CTSCSR0[CTSEN] and CTSCSR0[NTASKS] by the end of boot,
do not set it inside simulator CFG files!
