/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            type.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                zhaokai 2017
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifndef			_MINIOS_TYPE_H_
#define			_MINIOS_TYPE_H_

typedef unsigned int 		u32;
typedef unsigned short 		u16;
typedef unsigned char 		u8;

/*变长参数*/
typedef char*           va_list;
// 定义了一个返回 void，没有参数的函数指针
typedef void (*int_handler)();

typedef void (*task_f) ();
typedef void (*irq_handler)(int irq);
typedef void* system_call;
#endif