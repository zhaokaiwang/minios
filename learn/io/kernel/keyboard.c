/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                        keyboard.c
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                zhaokai,2017
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "keyboard.h"
#include "keymap.h"

PRIVATE KB_INPUT        kb_in;

PRIVATE	int	code_with_E0 = 0;
PRIVATE	int	shift_l;	/* l shift state */
PRIVATE	int	shift_r;	/* r shift state */
PRIVATE	int	alt_l;		/* l alt state	 */
PRIVATE	int	alt_r;		/* r left state	 */
PRIVATE	int	ctrl_l;		/* l ctrl state	 */
PRIVATE	int	ctrl_r;		/* l ctrl state	 */
PRIVATE	int	caps_lock;	/* Caps Lock	 */
PRIVATE	int	num_lock;	/* Num Lock	 */
PRIVATE	int	scroll_lock;	/* Scroll Lock	 */
PRIVATE	int	column;

PRIVATE u8      get_byte_from_kbuf();
PRIVATE void kb_wait();
PRIVATE void kb_ack();
PRIVATE void set_leds();
/* 键盘中断*/
PUBLIC  void keyboard_handler (int irq)
{
        u8 scan_code = in_byte (KB_DATA);

        // 缓冲区还没有满
        if (kb_in.count < KB_IN_BYTES){
                *(kb_in.p_head) = scan_code;
                kb_in.p_head ++;

                if (kb_in.p_head == kb_in.buf + KB_IN_BYTES){
                        kb_in.p_head = kb_in.buf;
                }
                kb_in.count ++;
        }
}
    
/* 初始化键盘中断和小键盘状态*/
PUBLIC  void init_keyboard()
{       
        kb_in.count = 0;
        kb_in.p_head = kb_in.p_tail = kb_in.buf;

        shift_l = shift_r = 0;
        alt_l = alt_r = 0;
        ctrl_l = ctrl_r = 0;

        caps_lock = 0;
        num_lock = 1;
        scroll_lock = 0;

        put_irq_handler(KEYBOARD_IRQ, keyboard_handler);   // 设置键盘中断处理子程序
        enable_irq(KEYBOARD_IRQ);                           // 开启键盘中断
}
 
/* 读取键盘中断缓冲区
根据类型来判断输出内容
*/
PUBLIC  void keyboard_read(TTY* p_tty)
{
        u8 scan_code;
        char output[2];
        int make; /* 是否是make code*/

        u32     key = 0; /* 用一个整数来表示一个键
                          *
                          */
        u32*     keyrow; /* 指向某一行*/
        memset (output, 0, 2);
        /* 缓冲区中还有值*/
        if (kb_in.count > 0)
        {
                code_with_E0 = 0;

                scan_code = get_byte_from_kbuf();
                
                /* 下面开始解析扫描码*/
                if (scan_code == 0xE1){
                        int i;
                        u8 pausebrk_scode[] = { 0XE1, 0X1D, 0X45, 0XE1,
                                                0X9D, 0XC5};
                        
                        int is_pausebreak = 1;

                        for (int i = 1; i< 6; i ++){
                                if (get_byte_from_kbuf() != pausebrk_scode[i]){
                                        is_pausebreak = 0;
                                        break;
                                }
                        }
                        if (is_pausebreak) {
                                key = PAUSEBREAK;
                        }
                        
                } else if (scan_code == 0xe0){
                        scan_code = get_byte_from_kbuf();

                        /*print screen 被按下*/
                        if (scan_code == 0x2a){
                                if (get_byte_from_kbuf() == 0xE0){
                                        if (get_byte_from_kbuf() == 0x37){
                                                key = PRINTSCREEN;
                                                make = TRUE;
                                        }
                                }
                        }
                        /*print screen 被释放*/
                        if (scan_code == 0xb7){
                                if (get_byte_from_kbuf() == 0xE0){
                                        if (get_byte_from_kbuf() == 0xaa){
                                                key = PRINTSCREEN;
                                                make = FALSE;
                                        }
                                }
                        }
                        /*不是printscreen ，scancode是紧跟着0xe0的那个数*/
                        if (key == 0){
                                code_with_E0 = 1;
                        }

                } 
                if ((key != PAUSEBREAK) && (key != PRINTSCREEN)){
                        make = (scan_code & FLAG_BREAK) ? FALSE : TRUE;
                        
                        /* 定位到某一行*/
                        keyrow = &keymap[(scan_code & 0x7f) * MAP_COLS];
                        column = 0;
                        
                        int caps = shift_l || shift_r;
                        
                        if (caps_lock){
                                if ((keyrow[0] >= 'a') && (keyrow[0] <= 'z')){
                                        caps = !caps;
                                }
                        }
                        if (caps){
                                column = 1;
                        }
                        
                        if (code_with_E0){
                                column = 2;
                                code_with_E0 = 0;
                        }

                        key = keyrow[column];

                        switch (key){
                        case SHIFT_L:
                                shift_l = make;
                                break;
                        case SHIFT_R:
                                shift_r = make;
                                break;
                        case CTRL_L:
                                ctrl_l = make;
                                break;
                        case CTRL_R:
                                ctrl_r = make;
                                break;
                        case ALT_L:
                                alt_l = make;
                                break;
                        case ALT_R:
                                alt_r = make;
                                break;
                        case CAPS_LOCK:
                                if (make){
                                        caps_lock = !caps_lock;
                                        set_leds();
                                }
                                break;
                        case NUM_LOCK:
                                if (make){
                                       num_lock = !num_lock;
                                        set_leds();
                                }
                                break;
                        case SCROLL_LOCK:
                                if (make){
                                        scroll_lock = !scroll_lock;
                                        set_leds();
                                }
                                break;
                        default:
                                break;
                        }

                        if (make){
                                int pad = 0;

				/* 首先处理小键盘 */
				if ((key >= PAD_SLASH) && (key <= PAD_9)) {
					pad = 1;
					switch(key) {
					case PAD_SLASH:
						key = '/';
						break;
					case PAD_STAR:
						key = '*';
						break;
					case PAD_MINUS:
						key = '-';
						break;
					case PAD_PLUS:
						key = '+';
						break;
					case PAD_ENTER:
						key = ENTER;
						break;
					default:
						if (num_lock &&
						    (key >= PAD_0) &&
						    (key <= PAD_9)) {
							key = key - PAD_0 + '0';
						}
						else if (num_lock &&
							 (key == PAD_DOT)) {
							key = '.';
						}
						else{
							switch(key) {
							case PAD_HOME:
								key = HOME;
								break;
							case PAD_END:
								key = END;
								break;
							case PAD_PAGEUP:
								key = PAGEUP;
								break;
							case PAD_PAGEDOWN:
								key = PAGEDOWN;
								break;
							case PAD_INS:
								key = INSERT;
								break;
							case PAD_UP:
								key = UP;
								break;
							case PAD_DOWN:
								key = DOWN;
								break;
							case PAD_LEFT:
								key = LEFT;
								break;
							case PAD_RIGHT:
								key = RIGHT;
								break;
							case PAD_DOT:
								key = DELETE;
								break;
							default:
								break;
							}
						}
						break;
					}
				}
                                key |= shift_l ? FLAG_SHIFT_L : 0;
                                key |= shift_r ? FLAG_SHIFT_R  : 0;
                                key |= ctrl_l ? FLAG_CTRL_L : 0;
                                key |= ctrl_r ? FLAG_CTRL_R  : 0;
                                key |= alt_l ? FLAG_ALT_L : 0;
                                key |= alt_r ? FLAG_ALT_R  : 0;
                                
                                in_process(p_tty, key);
                        }
                } 
        }
                
}

/*==============================================================================
                get_byte_from_kbuf
                从键盘缓冲区读取下一个字符
===========================================================================*/
PRIVATE u8      get_byte_from_kbuf()
{
        u8      scan_code;

        while (kb_in.count <= 0) {}

        disable_int();

        scan_code = *(kb_in.p_tail);
        kb_in.p_tail ++;
        if (kb_in.p_tail == kb_in.buf + KB_IN_BYTES){
                kb_in.p_tail = kb_in.buf;
        }
        kb_in.count --;
        enable_int();

        return scan_code;

}

/* 等待输入缓冲区为空*/
PRIVATE void kb_wait()
{
        u8 kb_stat;

        do{
                kb_stat = in_byte(KB_CMD);
        }while (kb_stat & 0x02);
}

/* 收到ack*/
PRIVATE void kb_ack()
{
        u8 kb_read;

        do{
                kb_read = in_byte(KB_DATA);
        }while (kb_read != KB_ACK);
}

/* 设置灯的状态*/
PRIVATE void set_leds()
{
        u8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;

        kb_wait();
        out_byte(KB_DATA, LED_CODE);
        kb_ack();
        kb_wait();
        out_byte(KB_DATA, leds);
        kb_ack();
}