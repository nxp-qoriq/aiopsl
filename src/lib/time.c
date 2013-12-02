/**************************************************************************//*
 @file          time.c

 @brief         AIOP Service Layer Time Queries routines

*//***************************************************************************/

#include "common/time.h" 
time_t time(
    time_t *t) 
{
    /* to be done */
    UNUSED (t);
    return 0;
}
 
int gettimeofday(
    struct timeval *tv,
    struct timezone *tz)
{
    /* to be done */
    UNUSED (tz);
    UNUSED (tv);
    return 0;
}


