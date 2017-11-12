 /*++++++++++++++++++++++++++++++++++++++
            global.h
+++++++++++++++++++++++++++++++++++++++++++
                                    zhaokai 2017

*/
#ifdef  GLOBAL_VARIABLES_HERE
#undef  EXTERN
#define EXTERN
#endif  
#include "proc.h"
EXTERN  int     ticks;

EXTERN  int     disp_pos;
EXTERN  u8      gdt_ptr[6];
EXTERN  DESCRIPTOR gdt[GDT_SIZE];
EXTERN  u8      idt_ptr[6];
EXTERN  GATE    idt[IDT_SIZE];

EXTERN  TSS tss;
EXTERN  PROCESS*    p_proc_ready;

extern   PROCESS    proc_table[];
extern   char       task_stack[];
extern   TASK       task_table[];
extern   irq_handler    irq_table[];

EXTERN  u32     k_reenter;    
