 /*+++++++++++++++++++++++++++++++++++++++++++++
            global.h
++++++++++++++++++++++++++++++++++++++++++++++++
                                    zhaokai 2017
++++++++++++++++++++++++++++++++++++++++++++++++
*/
#ifndef __MINIOS_GLOBAL_H
#define __MINIOS_GLOBAL_H


#ifdef  GLOBAL_VARIABLES_HERE
#undef  EXTERN
#define EXTERN
#endif  
#include "type.h"
#include "console.h"
#include "tty.h"
#include "proc.h"
#include "protect.h"


EXTERN  int     ticks;

EXTERN  int     disp_pos;
EXTERN  u8      gdt_ptr[6];
EXTERN  DESCRIPTOR gdt[GDT_SIZE];
EXTERN  u8      idt_ptr[6];
EXTERN  GATE    idt[IDT_SIZE];

EXTERN  TSS tss;
EXTERN  struct proc*    p_proc_ready;

extern   PROCESS    proc_table[];
extern   char       task_stack[];
extern   TASK      user_proc_table[];
extern   TASK     task_table[];
extern   irq_handler    irq_table[];

EXTERN  u32     k_reenter;    

/*TTY*/
EXTERN  int nr_current_console;
extern  TTY tty_table[];
extern  CONSOLE console_table[];

#endif