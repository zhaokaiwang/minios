/*===========================================================
                    TTY.H
===============================================================
                                            ZHAOKAI, 2017
*/
#ifndef _MINIOS_TTY_H
#define _MINIOS_TTY_H
#include "type.h"
#include "console.h"
#include "proc.h"

#define NR_CONSOLES			3 /*number of consoles*/

#define TTY_FIRST       (tty_table)
#define TTY_END         (tty_table + NR_CONSOLES)
#define TTY_IN_BYTES      256  /*TTY BUFFER SIZE */
struct s_console;
struct proc;
/*tty*/

typedef struct s_tty{
        u32     in_buf[TTY_IN_BYTES];  /*tty  缓冲区*/
        u32*    p_inbuf_head;          /* inbuf 指向缓冲区下一个可用的位置*/ 
        u32*    p_inbuf_tail;           /* 指向缓冲区最后的位置*/
        int     inbuf_count;            /* 缓冲区存放的数量*/

        struct s_console*   p_console;
}TTY;
    

/* 函数原型*/
PUBLIC  void task_tty();
PUBLIC void in_process(TTY* p_tty, u32 key);
PUBLIC void tty_write(TTY* p_tty, char* buf, int len);
PUBLIC void init_screen(TTY* p_tty);
PUBLIC int sys_printx( int _unused1, int _unused2, char* s, struct proc* p_proc);
#endif  /*TTY*/