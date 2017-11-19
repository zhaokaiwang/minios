/*===========================================================
                    CONSOLE.H
===============================================================
                                            ZHAOKAI, 2017
*/
#ifndef _MINIOS_CONSOLE_H
#define _MINIOS_CONSOLE_H
#include "type.h"

typedef struct s_console{
        unsigned int    current_start_addr;   /*当前显示的位置*/
        unsigned int    original_addr;        /*最开始的位置*/
        unsigned int    v_mem_limit;          /*现存的大小的限制*/
        unsigned int    cursor;               /*当前光标的位置*/
}CONSOLE;

#define SCR_UP	1	/* scroll forward */
#define SCR_DN	-1	/* scroll backward */

#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH		80

#define DEFAULT_CHAR_COLOR      0X07 /*0000 0111 黑底白字*/

#endif  /*CONSOLE*/