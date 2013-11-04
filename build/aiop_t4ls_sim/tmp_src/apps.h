#ifndef __APPS_H
#define __APPS_H


extern int aiop_app_init(void); extern void aiop_app_free(void);


#define APPS                            \
{                                       \
    {aiop_app_init, aiop_app_free},     \
    {NULL, NULL} /* never remove! */    \
}


#endif /* __APPS_H */
