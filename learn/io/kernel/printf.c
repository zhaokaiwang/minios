/*=====================================================================================
                                            print.c
======================================================================================
                                            zhaokai,2017
*/
#include "type.h"
#include "printf.h"
#include "vsprintf.h"
#include "proto.h"
/*格式化输入函数*/
int printf (const char* fmt,...)
{
    int i;
    char buf[256];

    va_list arg = (va_list)((char*) (&fmt) + 4);
    i = vsprintf(buf, fmt, arg);
    buf[i] = 0;
    printx(buf);
    
    return i;
}