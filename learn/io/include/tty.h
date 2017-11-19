/*===========================================================
                    TTY.H
===============================================================
                                            ZHAOKAI, 2017
*/
#ifndef _MINIOS_TTY_H
#define _MINIOS_TTY_H
#include "type.h"
#include "console.h"

#define TTY_IN_BYTES      256  /*TTY BUFFER SIZE */
struct s_console;
/*tty*/

typedef struct s_tty{
        u32     in_buf[TTY_IN_BYTES];  /*tty  缓冲区*/
        u32*    p_inbuf_head;          /* inbuf 指向缓冲区下一个可用的位置*/ 
        u32*    p_inbuf_tail;           /* 指向缓冲区最后的位置*/
        int     inbuf_count;            /* 缓冲区存放的数量*/

        struct s_console*   p_console;
}TTY;
    

#endif  /*TTY*/