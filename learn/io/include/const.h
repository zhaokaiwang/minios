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
#define	FALSE	0

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


/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40 /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43 /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :
			     * Counter0 - LSB then MSB - rate generator - binary
			     */
#define TIMER_FREQ     1193182L/* clock frequency for timer in PC and AT */
#define HZ             100  /* clock freq (software settable on IBM-PC) */

/* AT keyboard */
/* 8042 ports */
#define KB_DATA		0x60	/* I/O port for keyboard data
					Read : Read Output Buffer
					Write: Write Input Buffer(8042 Data&8048 Command) */
#define KB_CMD		0x64	/* I/O port for keyboard command
					Read : Read Status Register
					Write: Write Input Buffer(8042 Command) */

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
#define	NR_SYS_CALL		2

/* vga related register*/
#define CRTC_ADDR_REG 	0X3D4   /* CRT controller regiters - addr regitter */
#define CRTC_DATA_REG	0X3D5	/* CRT controller register - data register*/
#define START_ADDR_H	0XC 	/* reg index of video mem start addr (msb)*/
#define START_ADDR_L	0XD		/* reg index of video mem start addr (lsb)*/
#define CURSOR_H		0XE		/* reg index of cursor position (msb)*/
#define CURSOR_L		0XF 	/* reg index of cursor position (lsb)*/
#define V_MEM_BASE 		0XB8000	/* base of color video memoty*/
#define V_MEM_SIZE		0X8000	/*32K 0XB8000H -> OXBFFFFH*/

/*TTY*/
#define NR_CONSOLES			3 /*number of consoles*/

/* keyboard.h*/
#define LED_CODE 	0XED
#define KB_ACK		0XFA
#endif
