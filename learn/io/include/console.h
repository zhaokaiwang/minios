/*===========================================================
                    CONSOLE.H
===============================================================
                                            ZHAOKAI, 2017
*/
#ifndef _MINIOS_CONSOLE_H
#define _MINIOS_CONSOLE_H
#include "type.h"
#include "tty.h"
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

/* vga related register*/
#define CRTC_ADDR_REG 	0X3D4   /* CRT controller regiters - addr regitter */
#define CRTC_DATA_REG	0X3D5	/* CRT controller register - data register*/
#define START_ADDR_H	0XC 	/* reg index of video mem start addr (msb)*/
#define START_ADDR_L	0XD		/* reg index of video mem start addr (lsb)*/
#define CURSOR_H		0XE		/* reg index of cursor position (msb)*/
#define CURSOR_L		0XF 	/* reg index of cursor position (lsb)*/
#define V_MEM_BASE 		0XB8000	/* base of color video memoty*/
#define V_MEM_SIZE		0X8000	/*32K 0XB8000H -> OXBFFFFH*/


/* Color */
/*
 * e.g. MAKE_COLOR(BLUE, RED)
 *      MAKE_COLOR(BLACK, RED) | BRIGHT
 *      MAKE_COLOR(BLACK, RED) | BRIGHT | FLASH
 */
#define BLACK   0x0     /* 0000 */
#define WHITE   0x7     /* 0111 */
#define RED     0x4     /* 0100 */
#define GREEN   0x2     /* 0010 */
#define BLUE    0x1     /* 0001 */
#define FLASH   0x80    /* 1000 0000 */
#define BRIGHT  0x08    /* 0000 1000 */
#define	MAKE_COLOR(x,y)	((x<<4) | y) /* MAKE_COLOR(Background,Foreground) */

#define DEFAULT_CHAR_COLOR	(MAKE_COLOR(BLACK, WHITE))
#define GRAY_CHAR		(MAKE_COLOR(BLACK, BLACK) | BRIGHT)
#define RED_CHAR		(MAKE_COLOR(BLUE, RED) | BRIGHT)

/* 函数原型*/
PUBLIC int is_current_console(CONSOLE* p_con);
PUBLIC  void out_char(CONSOLE* p_con, char ch);
PUBLIC void set_cursor (unsigned int position);
PUBLIC void select_console(int nr_console);

PUBLIC  void scroll_screen(CONSOLE* p_con, int direction);

#endif  /*CONSOLE*/