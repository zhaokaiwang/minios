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
#include "tty.h"

/* 函数的原型*/
PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_wirte(TTY* p_tty);
PRIVATE void tty_do_read(TTY* p_tty);
PRIVATE void put_key(TTY *p_tty, u32 key);

/* 任务进程*/
PUBLIC  void task_tty()
{       
        TTY*    p_tty;

        init_keyboard();
        /*初始化每个tty*/
        for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty ++){
                init_tty(p_tty);
                out_char(p_tty->p_console,'a');
        }
        select_console(0);
        while (1){
                for (p_tty = TTY_FIRST; p_tty < TTY_END; p_tty ++){
                        tty_do_read(p_tty);
                        tty_do_wirte(p_tty);
                }
        }

}

/*初始化一个TTY*/
PRIVATE void init_tty(TTY* p_tty)
{
        p_tty->inbuf_count = 0;
        p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

        init_screen(p_tty);
}

/* 读当前的控制台*/
PRIVATE void tty_do_read(TTY* p_tty)
{
        if (is_current_console(p_tty->p_console)) {
                keyboard_read(p_tty);
        }
}

/* 往当前控制台写内容*/
PRIVATE void tty_do_wirte(TTY* p_tty)
{
        if (p_tty->inbuf_count){
                char ch = *(p_tty->p_inbuf_tail);
                p_tty->p_inbuf_tail++;
                if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES){
                        p_tty->p_inbuf_tail = p_tty->in_buf;
                }
                p_tty->inbuf_count --;
                
                out_char(p_tty->p_console, ch);
        }
}

/*================================================================================
        in_process(u32)
===================================================================================*/
PUBLIC void in_process(TTY* p_tty, u32 key)
{
        char output[2] = {'\0', '\0'};

        if (!(key & FLAG_EXT)){
                if (p_tty->inbuf_count < TTY_IN_BYTES) {
                        *(p_tty->p_inbuf_head) = key;
                        p_tty->p_inbuf_head ++;
                        if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES){
                                p_tty->p_inbuf_head = p_tty->in_buf;
                        }
                        p_tty->inbuf_count++;
                }
        } else {
            int raw_code = key & MASK_RAW;
            switch (raw_code){
                case ENTER:
                        put_key(p_tty, '\n');
                        break;
                case BACKSPACE:
                        put_key(p_tty, '\b');
                        break;
                case UP: 
                        if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)){
                               scroll_screen(p_tty->p_console, SCR_DN); 
                        }
                        break;
                case DOWN:
                        if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)){
                               scroll_screen(p_tty->p_console, SCR_UP); 
                        }
                        break;
                case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
                case F12:
                        /*alt + F1~F12*/
                        if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)){
                                select_console(raw_code - F1);
                        }
                        break;
                default:
                        break;
            }
        }
}

/* 向缓冲区的队列中添加一个按键*/
PRIVATE void put_key(TTY *p_tty, u32 key)
{
        if (p_tty->inbuf_count < TTY_IN_BYTES){
                *(p_tty->p_inbuf_head) = key;
                p_tty->p_inbuf_head ++;
                if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES){
                        p_tty->p_inbuf_head = p_tty->in_buf;
                }
        }
        p_tty->inbuf_count ++;
}

/* 向控制台写内容*/
PUBLIC void tty_write(TTY* p_tty, char* buf, int len)
{
        char*   p = buf;
        int i = len;
        while (i){
                out_char(p_tty->p_console, *p++);
                i --;
        }
}

/********************************************************************
 *                      sys_printx
 * ******************************************************************/
PUBLIC int sys_printx( int _unused1, int _unused2, char* s, struct proc* p_proc)
{
        const char* p;
        char ch; 
        
        char reenter_err[] = "? k_reenter is incorrect for unknown reason";
        reenter_err[0] = MAG_CH_ASSERT;

        if (k_reenter == 0) { /* run in ring<1-3>*/
                p = va2la(proc2pid(p_proc), s);
        } else if (k_reenter > 0) /* 内核中被调用*/
                p = s;
        else 
                p = reenter_err;
        
        
        /* 如果在内核中发生了assert错误的话，我们打印之后就停止我们的内核
           如果在用户进程中的话，我们就选择在相应的控制台打印出内容，然后返回
        */        
        if ((*p == MAG_CH_PANIC) ||
             (*p == MAG_CH_ASSERT && p_proc_ready < &proc_table[NR_TASKS])) {
                disable_int();

                char * v = (char*)V_MEM_BASE;
                const char* q = p + 1;

                while (v < (char*)(V_MEM_BASE + V_MEM_SIZE)){
                        *v++ = *q++;
                        *v++ = RED_CHAR;
                        if (!*q) {  /* 遇到了 0 我们就需要换行*/
                            while (((int)v - V_MEM_BASE) % (SCREEN_WIDTH * 16)){ /* 填充一行的 0*/
                                v++;
                                *v++ = GRAY_CHAR;
                            }
                            q = p + 1;
                        }
                }

                __asm__ __volatile__("hlt");
        }

        while ((ch = *p++) != 0){
                if (ch == MAG_CH_ASSERT || ch == MAG_CH_PANIC)
                        continue ; /* 跳过标志位*/
                out_char(tty_table[p_proc->nr_tty].p_console,ch);
        }
        return 0;
} 