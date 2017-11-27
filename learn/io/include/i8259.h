/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              keyboard.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                        zhaokai, 2017
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef _MINIOS_I8259_H
#define _MINIOS_I8259_H
#include "type.h"

/* 8259A interrupt controller ports. */
#define INT_M_CTL	0X20		//主中断控制器的Io端口 master
#define	INT_M_CTLMASK	0X21		//主 设置位去屏蔽或者开启中断
#define	INT_S_CTL	0XA0		// 从中断k
#define INT_S_CTLMASK	0XA1 

#define	NR_IRQ	16

#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ	2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ	3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ	4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ	5	/* xt winchester */
#define	FLOPPY_IRQ	6	/* floppy disk */
#define	PRINTER_IRQ	7
#define	AT_WINI_IRQ	14	/* at winchester */

/* 函数原型*/
PUBLIC void init_8259A();
PUBLIC	void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);
#endif