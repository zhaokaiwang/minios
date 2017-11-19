/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            klib.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                            zhaokai 2017
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

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


// 简单的延迟函数
PUBLIC void delay(int time){
    int i, j, k;

    for (k = 0; k < time; k ++){
        for (int i = 0; i < 10; i ++){
            for (j = 0; j < 10000; j ++){}
        }
    }
}