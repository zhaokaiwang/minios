/*++++++++++++++++++++++++++++++++++++++++++
	cosnt.h 
+++++++++++++++++++++++++++++++++++++++++++++
								zhaokai 2017
++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef		_MINIOS_CONST_H
#define 	_MINIOS_CONST_H

#define  EXTERN	extern 

//函数类型
#define  	PUBLIC   //外部可见函数
#define		PRIVATE	static	//内部使用函数

/* Boolean */
#define	TRUE	1
#define	FLASE	0

//GDT 和 IDT 中描述符的个数
#define    	GDT_SIZE	128
#define		IDT_SIZE	256

/* 权限 */

#define PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK  1
#define PRIVILEGE_USER  3
/* RPL */
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3
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

/* system call */
#define	NR_SYS_CALL		1

/*8252 pit */
#define TIMER0 	0X40 /* I/O port for time channel 0*/
#define TIMER_MODE 	0X43		/* I/O PORT FOR TIMER MODE CONTROL*/
#define	RATE_GENERATOR	0X34		/*通过手册可以知道具体位的值*/
#define	TIMER_FREQ		1193182L   	/* 时钟频率*/
#define	HZ			100		
#endif
