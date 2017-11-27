/*++++++++++++++++++++++++++++++++++++++++++++++
            string.h
++++++++++++++++++++++++++++++++++++++++++++++++++
                            zhaokai,2017
++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "type.h"
PUBLIC void* memcpy(void* p_dest, void *p_src, int size);
PUBLIC void memset(void*, char, int);
PUBLIC char* strcpy(char*, char*);
PUBLIC int   strlen(char*);

#define	phys_copy	memcpy
#define	phys_set	memset