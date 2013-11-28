/**************************************************************************//*!
 @brief         inet.c

 @description   AIOP Service Layer Network Utilities implementation

 @cautions      None.
*//***************************************************************************/

#include "net/inet.h"

/***************************************************************************/
int inet_pton(
              int af, 
              const char *src,
	      void *dst
)
{
	/* to be done */
#pragma unused (af, dst, src) 
	return 0;
}

const char *inet_ntop(
    int af, 
    const void *src,                      char
    *dst, 
    size_t size
)
{
#pragma unused (af, dst, size, src) 
	/* to be done */
	return NULL;
}

