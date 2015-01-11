 - To run bring-up tests, replace the main.c file in the MC project with the file mc/tests/bringup/arena/bringup_main.c
 - To run AIOP bring-up tests also replace the aiop main with the file aiop/tests/bringup/bringup_main.c
   - In MC bringup_tests.h set TEST_AIOP = ON
   - If there are initiations in MC side, there should be a file aiop_XXX_init.c in MC git with those initializations.
 - bringup_tests.h will contain function declerations for all bringup related tests.
 - sys_is_master_core() API will work in tests (both initialization and test, MC and AIOP).
 
 for example see mem access test:
 MC: 
  - mem_access_test.c
 
 AIOP: 
  - aiop_mem_access_init.c (MC git)
  - mem_access_test.c (AIOP git)