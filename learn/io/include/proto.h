/*+++++++++++++++++++++++++++++++++++++++++++++++
            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++
                                zhaokai 2017
++++++++++++++++++++++++++++++++++++++++++++++*/


// klib.asm
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8 in_byte(u16 port);
PUBLIC void disp_str(char *info);
PUBLIC void disp_color_str(char *info, int color);
PUBLIC void init_prot();
PUBLIC void init_8259A();
PUBLIC void disp_int(int input);
PUBLIC void delay(int time);

PUBLIC  void    disable_int();
PUBLIC  void    enable_int();
void restart();
void testA();
void testB();

PUBLIC  void put_irq_handler(int, irq_handler);
PUBLIC  void spurious_irq(int irq);
PUBLIC  void clock_handler(int irq);
PUBLIC  void enable_irq(int);
PUBLIC  void disable_irq(int);

PUBLIC void milli_delay(int);
PUBLIC char *itoa(char* str, int num);
/* 系统调用相关 */
/* proc.c */
PUBLIC  int     sys_get_ticks();    /* sys_call */
PUBLIC  void    schedule();        /* 调度函数 */

/*syscall.asm*/
PUBLIC  void    sys_call();
PUBLIC  int     get_ticks();

/*clock.c*/
PUBLIC  void    init_clock();

/*keyboard.c*/
PUBLIC  void    init_keyboard();
PUBLIC  void    keyboard_read(TTY*);

/* tty.c*/
PUBLIC  void    task_tty();
PUBLIC  void    in_process(TTY*,u32);
PUBLIC void init_screen(TTY* p_tty);

/*console.c*/
PUBLIC int is_current_console(CONSOLE* p_con);
PUBLIC void out_char(CONSOLE* p_con, char ch);
PUBLIC void select_console(int nr_console);
PUBLIC void set_cursor (unsigned int position);
PUBLIC  void scroll_screen(CONSOLE* p_con, int direction);

/* printf.c */
PUBLIC  int     printf(const char *fmt, ...);

/* vsprintf.c */
PUBLIC  int     vsprintf(char *buf, const char *fmt, va_list args);

/* 以下是系统调用相关 */

/* 系统调用 - 系统级 */
/* proc.c */
PUBLIC  int     sys_get_ticks();
PUBLIC  int     sys_write(char* buf, int len, PROCESS* p_proc);
/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */

/* 系统调用 - 用户级 */
PUBLIC  int     get_ticks();
PUBLIC  void    write(char* buf, int len);