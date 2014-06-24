/**************************************************************************//**

 @File          list.c

 @Description   Implementation of list.
*//***************************************************************************/

#include "fsl_list.h"


void list_append(list_t *p_new_list, list_t *p_head)
{
    list_t *p_first = LIST_FIRST(p_new_list);

    if (p_first != p_new_list)
    {
        list_t *p_last  = LIST_LAST(p_new_list);
        list_t *p_cur   = LIST_NEXT(p_head);

        LIST_PREV(p_first) = p_head;
        LIST_FIRST(p_head) = p_first;
        LIST_NEXT(p_last)  = p_cur;
        LIST_LAST(p_cur)   = p_last;
    }
}


int list_num_of_objs(list_t *p_list)
{
    list_t *p_tmp;
    int    num_of_objs = 0;

    if (!list_is_empty(p_list))
        LIST_FOR_EACH(p_tmp, p_list)
            num_of_objs++;

    return num_of_objs;
}
