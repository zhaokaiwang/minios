/*====================================================================
                                            vsprintf.c
========================================================================
                                            zhaokai 2017
*/
#include "type.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

#define STR_DEFAULT_LEN 1024

/***********************************************************************
 *                              i2a
 * **ps 使用二阶指针是为了变动指向 ps 的值
 ***********************************************************************/
PRIVATE char* i2a (int val, int base, char **ps)
{
        int  m = val % base;
        int  q = val / base;
        if (q) {
                i2a(q, base, ps);
        }
        *(*ps) ++ = (m < 10) ? (m + '0'): (m - 10 + 'A');
        return *ps;
        
}
/**************************************************************
 *              vsprintf
 * 在这里每次都是我们字节变长参数的格式化输出，在这里我们自己遍历了每一个参数
 * 也说明了说明了每个都是靠%%号的个数来判断参数的个数的
 * */
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
        char* p;
        char    inner_buf[STR_DEFAULT_LEN];
        va_list p_next_arg = args;
        int     align_nr;    /* align*/
        char    cs;
        int m ;

        for (p=buf; *fmt; fmt++){
            if (*fmt != '%'){
                *p++ = *fmt;
                continue;
            } else {
                align_nr = 0;
            }

            fmt ++;

            if (*fmt == '%'){ /* case: "%%"*/
                *p++ = *fmt;
                continue;
            } else if (*fmt == '0'){
                cs = '0';
                fmt ++;
            } else {
                cs = ' ';
            }
            
            /* 取出 "**D" */
            while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')){
                align_nr *= 10;
                align_nr += *fmt - '0';
                fmt ++;
            }

            char* q = inner_buf;
            memset (q, 0, sizeof(inner_buf));

            switch(*fmt) {
            case 'c':
                *q++ =  *((char*)p_next_arg);
                p_next_arg += 4;
                break;
            case 'x':
                m = *((int*)p_next_arg);
                i2a(m, 16, &q);
                break;
            case 'd':
                m = *((int*)p_next_arg);
                if (m < 0){
                        m = -m ;
                        *q ++ = '-';
                }
                i2a(m, 10, &q);
                p_next_arg += 4;
                break;
             case 's':
                strcpy(q, *((char**)p_next_arg));
                q += strlen(p_next_arg);
                p_next_arg += 4;
             default:
                break;
                }
        
                int count = align_nr > strlen(inner_buf)? (align_nr - strlen(inner_buf)): 0;
                /* 填充对齐*/
                for (int k = 0; k < count; k ++){
                        *p++ = cs;
                } 
                q = inner_buf;
                while (*q) {
                        *p ++ = *q ++;
                }

        }
        /*字符串结束标志*/
        *p = 0;
        return (p - buf);
}

/*****************************************************************************************
 *                              sprintf                                                 
 *************************************************************************************/
int sprintf (char* buf, const char *fmt, ...)
{
        va_list arg = (va_list)((char*)(&fmt) + 4);             /* 类似二维数组的下一项*/
        return vsprintf(buf, fmt, arg);
}


                                    