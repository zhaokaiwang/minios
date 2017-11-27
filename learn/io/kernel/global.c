/*++++++++++++++++++++++++++++++++++++++++++++++++
        global.c
++++++++++++++++++++++++++++++++++++++++++++++++++
                            zhaokai, 2017
+++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "main.h"
#include "i8259.h"
#include "systask.h"

//进程表和任务表
PUBLIC struct proc proc_table[NR_TASKS + NR_PROCS];

// 进程栈的总大小
PUBLIC char     task_stack[STACK_SIZE_TOTAL];

PUBLIC struct task task_table[NR_TASKS] = {{task_tty, STACK_SIZE_TTY, "tty"},
                                          {task_sys, STACK_SIZE_SYS, "SYS"}};

PUBLIC  struct task user_proc_table[NR_PROCS] = {{testA, STACK_SIZE_TESTA, "testA"},
                                        {testB, STACK_SIZE_TESTB, "testB"}};

PUBLIC irq_handler irq_table[NR_IRQ];
PUBLIC TTY      tty_table[NR_CONSOLES];
PUBLIC CONSOLE  console_table[NR_CONSOLES];

PUBLIC	system_call	sys_call_table[NR_SYS_CALL] = {sys_printx,
						       sys_sendrec};