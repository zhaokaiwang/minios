/*+++++++++++++++++++++++++++++++++++++++++++++++
            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++
                                zhaokai 2017
++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef _MINIOS_PROTO_H
#define _MINIOS_PROTO_H
#include "type.h"

// klib.asm
PUBLIC void out_byte(u16 port, u8 value);
PUBLIC u8 in_byte(u16 port);
PUBLIC void disp_str(char *info);
PUBLIC void disp_color_str(char *info, int color);
PUBLIC void disp_int(int input);

PUBLIC  void    disable_int();
PUBLIC  void    enable_int();
void restart();

PUBLIC  void put_irq_handler(int, irq_handler);
PUBLIC  void spurious_irq(int irq);
PUBLIC  void enable_irq(int);
PUBLIC  void disable_irq(int);

PUBLIC char *itoa(char* str, int num);

/* syscall*/
PUBLIC int printx(char*);
PUBLIC int sendrec(int, int, MESSAGE*);

/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
#endif