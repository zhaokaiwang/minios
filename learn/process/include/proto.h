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
void restart();
void testA();
void testB();

PUBLIC  void put_irq_handler(int, irq_handler);
PUBLIC  void spurious_irq(int irq);
PUBLIC  void clock_handler(int irq);
PUBLIC  void enable_irq(int);
PUBLIC  void disable_irq(int);

PUBLIC void milli_delay(int);

/* 系统调用相关 */
/* proc.c */
PUBLIC  int     sys_get_ticks();    /* sys_call */
PUBLIC  void    schedule();        /* 调度函数 */

/*syscall.asm*/
PUBLIC  void    sys_call();
PUBLIC  int     get_ticks();

