/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            klib.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                            zhaokai 2017
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "global.h"


/*-------------------------------------------------------
                itoa 
----------------------------------------------------*/
// 数字前面的 0 不被显示出来，
// 数字转字符串
PUBLIC char *itoa(char* str, int num)
{
    char *p = str;
    char ch;
    int i = 0;
    int flag = 0;

    *p++ = '0';
    *p++ = 'x';

    if (num == 0){
            *p++ = '0';
    }else {
        for (int i = 28; i >= 0; i -= 4){
            ch = (num >> i) & 0xf;
           
            // flag 为了选出大于 0 的位
            if (flag || (ch > 0)){
                flag = 1;
                ch += '0';
                if (ch > '9'){
                    ch += 7;
                }
                *p++ = ch;
            }
        }
    }
    
    // 终止符
    *p = 0;
    
    return str;
}



//disp_int
PUBLIC void disp_int(int input)
{
    char output[16];
    itoa(output, input);
    disp_str(output);
}