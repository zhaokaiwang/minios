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

//GDT 和 IDT 中描述符的个数
#define    	GDT_SIZE	128
#define		IDT_SIZE	256

/* 权限 */

#define PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK  1
#define PRIVILEGE_USER  3

/* 8259A interrupt controller ports. */
#define INT_M_CTL	0X20		//主中断控制器的Io端口 master
#define	INT_M_CTLMASK	0X21		//主 设置位去屏蔽或者开启中断
#define	INT_S_CTL	0XA0		// 从中断k
#define INT_S_CTLMASK	0XA1 

#endif
