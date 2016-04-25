 - To run bring-up tests, the MC project is used as is, no changes required
 - To run AIOP bring-up, use the bringup_tests project. Each test has its own target

The target TEST_BIG has all the tests as one, except the exception tests:
TEST_MEM_ACCESS
TEST_MULTI_CLUSTER
TEST_SPINLOCKS
TEST_DPNI
TEST_BUFFER_POOLS
TEST_GPP_DDR.
