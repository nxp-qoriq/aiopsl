This folder contains helper functions to implement various workarounds to fix the default frame descriptor.
These functions are useful in scenarios that involve IP reassembly + IPSec + IP fragmentation.
These functions are not optimized for performance.

The functions have two versions:
- with CDMA R/W operations,
- with CPUs R/W operations.
  It imposes the buffers are allocated from a memory partition mapped into AIOP core virtual space:
    - buffers from PEB; this should work because PEB memory is mapped into AIOP virtual space
    - buffers from DP-DDR or from SYS-DDR; this works if the application uses up to 3 GB from those partitions.
  Use: #define CORE_VIRTUAL_ACCESS

