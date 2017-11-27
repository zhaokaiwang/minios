
#include "type.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "main.h"
#include "printf.h"
#include "clock.h"
#include "keyboard.h"

void testA()
{
    int i = 0;
    while (1){
        printf("<ticks:%x>", get_ticks());
        milli_delay(200);
    }
}


void testB(){
    while (1){
        printf("B.");
        milli_delay(200);
    }
}

PUBLIC int get_ticks()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}

// KERNEL 开始的地方
PUBLIC int kernel_main()
{
    disp_str("-------------------------kernel_main begins-----------\n");

    struct task*   p_task      = task_table;
    PROCESS* p_proc     = proc_table;
    char*   p_task_stack    = task_stack + STACK_SIZE_TOTAL;
    u16     selector_ldt    = SELECTOR_LDT_FIRST;

    u8          privilge;
    u8          rpl;
    int         eflags;
    int i = 0;
    int prio;

    for (; i < NR_TASKS + NR_PROCS; i++) {
        p_proc->nr_tty = 0;
        if (i < NR_TASKS){                   /* 任务进程*/
            p_task = task_table + i;
            privilge = PRIVILEGE_TASK;
            rpl = RPL_TASK;
            eflags = 0x1202;
            prio = 15;
        } else {
            p_task = user_proc_table + i - NR_TASKS;
            privilge = PRIVILEGE_USER;
            rpl = RPL_USER;
            eflags = 0x202;
            prio = 5;
        }

        strcpy (p_proc->name, p_task->name);
        p_proc->pid = i;

        p_proc->ldt_sel = selector_ldt;

        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof (DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | privilge << 5; // 改变 DPL
        
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof (DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | privilge << 5; //数据段 DPL

        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
        
        p_proc->regs.eip = (u32) p_task->initial_eip;
        p_proc->regs.esp = (u32) p_task_stack;
        p_proc->regs.eflags = eflags; //IF = 1, IOPL = 1, BIT2 is always 1.
        
        p_proc->nr_tty = 0;

        p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;
        
        p_proc->ticks = p_proc->priority = prio;

        p_task_stack -= p_task->stacksize;
        p_proc ++;
        p_task ++;
        selector_ldt += 1 << 3;
    } 

    proc_table[NR_TASKS + 0].nr_tty  = 1;
    proc_table[NR_TASKS + 1].nr_tty  = 1;

    k_reenter = 0;
    ticks = 0;
    /*键盘 和时钟 中断*/
    init_clock();
    init_keyboard();

    p_proc_ready = proc_table;
    restart();
        
        while (1){}
}