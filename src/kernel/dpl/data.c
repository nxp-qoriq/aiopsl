#include "common/types.h"
#include "common/fsl_stdlib.h"

#include "dtc.h"

#if 0
// TODO removed because it's not used
struct data data_grow_for(struct data d, int xlen)
{
    struct data nd;
    int newsize;

    if (xlen == 0)
        return d;

    nd = d;

    newsize = xlen;

    while ((d.len + xlen) > newsize)
        newsize *= 2;

    nd.val = realloc(d.val, newsize);

    return nd;
}

#endif
