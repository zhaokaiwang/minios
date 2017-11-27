/*+++++++++++++++++++++++++++++++++++++++++++++++
            printf.h
++++++++++++++++++++++++++++++++++++++++++++++++
                                zhaokai 2017
++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef _MINIOS_PRINTF_H
#define _MINIOS_PRINTF_H
#include "type.h"
PUBLIC int printf (const char* fmt,...);
#define printl printf
#endif