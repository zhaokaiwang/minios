/*++++++++++++++++++++++++++++++++++++++++++++++++
        global.c
++++++++++++++++++++++++++++++++++++++++++++++++++
                            zhaokai, 2017
+++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef  _MINIOS_GLOBAL_H
#define _MINIOS_GLOBAL_H
#define GLOBAL_VARIABLES_HERE

#include    "type.h"
#include    "const.h"
#include    "protect.h"
#include    "proto.h"
#include    "proc.h"
#include    "global.h"

//进程表
PUBLIC PROCESS proc_table[NR_TASKS];

// 进程栈的总大小
PUBLIC char     task_stack[STACK_SIZE_TOTAL];

PUBLIC TASK     task_table[NR_TASKS] = {{testA, STACK_SIZE_TESTA, "testA"},
                                        {testB, STACK_SIZE_TESTB, "testB"}};
                                        
PUBLIC system_call      sys_call_table[NR_SYS_CALL] = {sys_get_ticks};

PUBLIC irq_handler irq_table[NR_IRQ];
#endif