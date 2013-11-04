#include "common/types.h"
#include "common/fsl_stdio.h"


int aiop_app_init(void);
void aiop_app_free(void);


/* Command Portal structure */
typedef struct t_CmdPortal {
    uint32_t    cmdWord;
    uint32_t    param1;     /* Parameter 1: Command number */
    uint64_t    param2;     /* Parameter 2: Value */
    uint64_t    param3;     /* Parameter 3: Command dependent */
    uint64_t    param4;     /* Parameter 4: Return value */
    uint64_t    reserved[4];

} t_CmdPortal;


static int32_t test_mc_portal_cmd1(int32_t portal_num, int32_t highPrio)
{
    int32_t     i = 0;
    uint32_t    MC_DMEM_ADDR_BASE = 0x80000000;
    uint32_t    MC_PORTAL_SIZE = 0x10000;
    t_CmdPortal *p_CmdPortal = (t_CmdPortal *)(MC_DMEM_ADDR_BASE + (portal_num * MC_PORTAL_SIZE));
    uint64_t    expectedRetVal;

    fsl_os_print("Write mc_portal_cmd1 to : 0x%08lx ... ", (uint32_t)p_CmdPortal);

    p_CmdPortal->param1 = 0x1;
    p_CmdPortal->param2 = 0x0000000F0000000ALL;
    p_CmdPortal->param3 = 0;
    p_CmdPortal->param4 = 0;

    if (highPrio)
        p_CmdPortal->cmdWord = 0x00800000;
    else
        p_CmdPortal->cmdWord = 0x0;

    do
    {
        i++;
    } while (p_CmdPortal->param4 == 0);

    fsl_os_print("Done\n");

    /* Checking return value */
    expectedRetVal = (highPrio ? p_CmdPortal->param2 - 1 : p_CmdPortal->param2 + 1);
    if (p_CmdPortal->param4 != expectedRetVal)
        return 1;

    return 0;
}


int aiop_app_init(void)
{
    int err = 0;

    fsl_os_print("TEST: AIOP+MC\n");

    // write mc portal 0
    err = test_mc_portal_cmd1(0, 1);

    return err;
}

void aiop_app_free(void)
{
    /* TODO - complete!*/
}
