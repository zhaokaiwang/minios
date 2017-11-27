/*==================================================================
                                    tty.c
====================================================================
                                            zhaokai, 2017
====================================================================
*/
#include "type.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

/*函数原型*/
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE* p_con);

/* 当前输出的是当前控制台*/
PUBLIC int is_current_console(CONSOLE* p_con)
{
    return (p_con == &console_table[nr_current_console]);
}

/* 输出一个字符到控制台*/
PUBLIC  void out_char(CONSOLE* p_con, char ch)
{
        u8* p_vmem = (u8*)(V_MEM_BASE + p_con->cursor * 2);

        switch(ch){
            case '\n':
                if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - SCREEN_WIDTH){
                        p_con->cursor = p_con->original_addr + SCREEN_WIDTH * 
                        ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH + 1);
                }
                break;
            case '\b':
                if (p_con->cursor > p_con->original_addr){
                    p_con->cursor --;
                    *(p_vmem - 2) = ' ';
                    *(p_vmem - 1) = DEFAULT_CHAR_COLOR;
                }
                break;
            default:
                if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit -1){
                    *p_vmem++ = ch;
                    *p_vmem++ = DEFAULT_CHAR_COLOR;
                    p_con->cursor ++;     
                }
                break;
        }
        
        while (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE){
                scroll_screen(p_con, SCR_DN);
        }
        
        flush(p_con);
}

PRIVATE void flush(CONSOLE* p_con)
{
        set_cursor(p_con->cursor);
        set_video_start_addr(p_con->current_start_addr);
}


/*===========================================================
                            set_cursor
================================================================*/
PUBLIC void set_cursor (unsigned int position)
{
        disable_int();
        out_byte(CRTC_ADDR_REG, CURSOR_H);
        out_byte(CRTC_DATA_REG, (position >>8) & 0Xff);
        out_byte(CRTC_ADDR_REG, CURSOR_L);
        out_byte(CRTC_DATA_REG, position & 0XFF);
        enable_int();
}
 
/*选择控制台*/
PUBLIC void select_console(int nr_console)
{
        if ((nr_console < 0) || (nr_console >= NR_CONSOLES))
            return ;
        
        nr_current_console = nr_console;
        set_cursor(console_table[nr_console].cursor);
        set_video_start_addr(console_table[nr_console].current_start_addr);
}
    
/*设置现存的开始地址*/
PRIVATE void set_video_start_addr(u32 addr)
{
        disable_int();
        out_byte(CRTC_ADDR_REG, START_ADDR_H);
        out_byte(CRTC_DATA_REG, (addr >> 8) & 0Xff);
        out_byte(CRTC_ADDR_REG, START_ADDR_L);
        out_byte(CRTC_DATA_REG, addr & 0XFF);
        enable_int();
}    
    
/* 初始化屏幕控制台*/
PUBLIC void init_screen(TTY* p_tty)
{
    int nr_tty = p_tty - tty_table;
    p_tty->p_console = console_table + nr_tty;

    int v_mem_size = V_MEM_SIZE >> 1;

    int con_v_mem_size =  v_mem_size / NR_CONSOLES;
    CONSOLE *con = p_tty->p_console;
    con->original_addr = nr_tty * con_v_mem_size;
    con->v_mem_limit = con_v_mem_size;
    con->current_start_addr = con->original_addr;

    /*默认光标在最开始的地方*/
    con->cursor = con->original_addr;
    if (nr_tty == 0){
        con->cursor = disp_pos / 2;
        disp_pos = 0;
    } else {
        out_char(con, nr_tty + '0');
        out_char(con, '#');
    }
    set_cursor(con->cursor);
} 

/*向上或者向下滚动屏幕*/
PUBLIC  void scroll_screen(CONSOLE* p_con, int direction)
{
    if (direction == SCR_UP){
            if (p_con->current_start_addr > p_con->original_addr){
                    p_con->current_start_addr -= SCREEN_WIDTH;
            }
    } else if (direction == SCR_DN){
            if (p_con->current_start_addr + SCREEN_SIZE < 
                p_con->original_addr + p_con->v_mem_limit){
                    p_con->current_start_addr += SCREEN_WIDTH;
                }
            }
            
    set_video_start_addr(p_con->current_start_addr);
    set_cursor(p_con->cursor);
}