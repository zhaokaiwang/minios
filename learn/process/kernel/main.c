
//测试第一个进程，只有一个进程，我们在这里测试他是否可以运行
#include    "type.h"
#include    "const.h"
#include    "protect.h"
#include    "proto.h"
#include    "global.h"
#include    "proc.h"
#include    "string.h"

void testA()
{
    int i = 0;
    while (1){
        disp_str("A.");
        milli_delay(10);
    }
}


void testB(){
    int i = 0x1000;
    while (1){
        disp_str("B.");
        milli_delay(10);
    }
}

// KERNEL 开始的地方
PUBLIC int kernel_main()
{
    k_reenter = 0;
    ticks = 0;
    disp_str("-------------------------kernel_main begins-----------\n");

    TASK*   p_task      = task_table;
    PROCESS* p_proc = proc_table;
    char*   p_task_stack    = task_stack + STACK_SIZE_TOTAL;
    u16     selector_ldt    = SELECTOR_LDT_FIRST;
    int i = 0;

    /* 初始化定时器模块的地方 */
    out_byte (TIMER_MODE, RATE_GENERATOR);
    out_byte (TIMER0, (u8) (TIMER_FREQ /HZ));
    out_byte (TIMER0, (u8) (TIMER_FREQ / HZ) >> 8);    

    for (; i < NR_TASKS; i++) {
        strcpy (p_proc->p_name, p_task->name);
        p_proc->pid = i;

        p_proc->ldt_sel = selector_ldt;

        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof (DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5; // 改变 DPL
        
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof (DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5; //数据段 DPL

        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | RPL_TASK;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | RPL_TASK;
        p_proc->regs.eip = (u32) p_task->initial_eip;
        p_proc->regs.esp = (u32) p_task_stack;
        p_proc->regs.eflags = 0x1202; //IF = 1, IOPL = 1, BIT2 is always 1.
        
        proc_table[0].ticks = proc_table[0].priority = 15;
        proc_table[1].ticks = proc_table[1].priority = 5;

        p_task_stack -= p_task->stacksize;
        p_proc ++;
        p_task ++;
        selector_ldt += 1 << 3;
    }
    put_irq_handler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);
    p_proc_ready = proc_table;
    restart();
        
        while (1){}
}