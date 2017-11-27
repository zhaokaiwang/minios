/*-----------------------------------------------------------------
    proc.h
-------------------------------------------------------------------
                                                zhaokai 2017
----------------------------------------------------------------------
*/

#ifndef _MINIOS_PROC_H
#define _MINIOS_PROC_H
#include "type.h"
#include "protect.h"

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
typedef struct proc{
    STACK_FRAME regs;   /* 寄存器的内容保存在栈帧里面 */

    u16 ldt_sel;        /*gdt 选择子*/
    DESCRIPTOR ldts[LDT_SIZE]; // 保存进程的代码段和数据段

    int ticks;              /* 可供进程使用的ticks*/
    int priority;           /* 进程的优先级*/
    u32 pid;            //进程的ID
    char name[16];    //进程的名字
    int p_flags;        /*
                        *process flags 
                        *A proc is runnable if p_flags ==0
                        */
    MESSAGE* p_msg;
    int p_recvfrom;
    int p_sendto;

    int has_int_msg;    /**
                        * 如果中断发生了，但是任务还没有准备好
                        * 值非0
                        */

    struct proc* q_sending; /**
                            * 发送消息到这个进程的
                            */
    struct proc* next_sending; /**
                                *下一个进程在队列中的
                                */

    int nr_tty;         //输出的控制台的编号
}PROCESS;


// 声明了任务的结构体
typedef struct task{
    task_f  initial_eip;
    int     stacksize;
    char    name[32];
}TASK;

#define proc2pid(x) (x - proc_table)

// 任务的个数
#define NR_TASKS    2
#define NR_PROCS    2

#define FIRST_PROC  proc_table[0]
#define LAST_PROC   proc_table[NR_TASKS + NR_PROCS - 1]

// 任务的栈
#define STACK_SIZE_TESTA    0X8000
#define STACK_SIZE_TESTB    0x8000
#define STACK_SIZE_TTY      0X8000
#define STACK_SIZE_SYS      0X8000

#define STACK_SIZE_TOTAL    (STACK_SIZE_TESTA  + STACK_SIZE_TESTB + STACK_SIZE_TTY \
                            + STACK_SIZE_SYS)

#define INVALID_DRIVER	-20
#define INTERRUPT	-10
#define TASK_TTY	0
#define TASK_SYS	1
/* #define TASK_WINCH	2 */
/* #define TASK_FS	3 */
/* #define TASK_MM	4 */
#define ANY		(NR_TASKS + NR_PROCS + 10)
#define NO_TASK		(NR_TASKS + NR_PROCS + 20)
#define proc2pid(x) (x - proc_table)

/* ipc */
#define SEND		1
#define RECEIVE		2
#define BOTH		3	/* BOTH = (SEND | RECEIVE) */

/* magic chars used by `printx' */
#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'

/**
 * @enum msgtype
 * @brief MESSAGE types
 */
enum msgtype {
	/* 
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS,
};

#define	RETVAL		u.m3.m3i1

/* Process */
#define SENDING   0x02	/* set when proc trying to send */
#define RECEIVING 0x04	/* set when proc trying to recv */


#define	NR_SYS_CALL		2
/* 函数原型*/
PUBLIC void schedule();
PUBLIC int sys_sendrec(int, int, MESSAGE*,struct proc *);
PUBLIC u32 ldt_seg_linear(struct proc *p, int index);
PUBLIC void* va2la(int pid, void* va);
PUBLIC int send_recv(int function, int src_dest, MESSAGE* msg);
PUBLIC void reset_msg(MESSAGE *m);
#endif 