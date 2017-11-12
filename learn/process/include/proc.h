/*-----------------------------------------------------------------
    proc.h
-------------------------------------------------------------------
                                                zhaokai 2017
----------------------------------------------------------------------
*/

#ifndef _MINIOS_PROC_H
#define _MINIOS_PROC_H

//栈帧的声明,保存寄存器值的地方
typedef struct s_stackframe {
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 kernel_esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 retaddr;
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;

}STACK_FRAME;


// 一个进程所有的内容保存在这里
typedef struct s_proc{
    STACK_FRAME regs;   /* 寄存器的内容保存在栈帧里面 */

    u16 ldt_sel;        /*gdt 选择子*/
    DESCRIPTOR ldts[LDT_SIZE]; // 保存进程的代码段和数据段

    int ticks;              /* 可供进程使用的ticks*/
    int priority;           /* 进程的优先级*/
    u32 pid;            //进程的ID
    char p_name[16];    //进程的名字

}PROCESS;


// 声明了任务的结构体
typedef struct s_task{
    task_f  initial_eip;
    int     stacksize;
    char    name[32];
}TASK;

// 任务的个数
#define NR_TASKS    2

// 任务的栈
#define STACK_SIZE_TESTA    0X8000
#define STACK_SIZE_TESTB    0x8000

#define STACK_SIZE_TOTAL    (STACK_SIZE_TESTA  + STACK_SIZE_TESTB)
#endif 