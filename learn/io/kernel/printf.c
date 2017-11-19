/*=====================================================================================
                                            print.c
======================================================================================
                                            zhaokai,2017
*/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
/*格式化输入函数*/
int printf (const char* fmt,...)
{
    int i;
    char buf[256];

    va_list arg = (va_list)((char*) (&fmt) + 4);
    i = vsprintf(buf, fmt, arg);
    write (buf, i);
    
    return i;
}