#编程控制中断，区分保护模式和实模式
    
  实模式下面通过中断向量表来完成，保护模式通过改变IDT来完成，IDT也是一种描述符，可以是下面三种。
* 中断门描述符
* 陷阱们描述符
* 任务门描述符

  通过设置描述符来完成中断寻找中断处理程序的过程。
  
## 异常
  异常分为三种。
  
* fault是一种可以可以被更正的错误，一旦被更正，程序可一不失连续性的继续执行。当一个fault发生时，处理起会把产生fault的指令之前的状态保存起来。异常处理程序的返回地址是产生fault的那条指令。
* trap是一种发生trap指令之后立即被报告的异常，同样支持程序不失连续的继续执行。异常处理程序的返回地址是产生trap指令之后的那条指令。
* abort是一种不总是报告精确异常发生位置的异常，它不允许程序或者任务继续执行，而是用来报告严重错误的。

## 外部中断和内部中断
   外部中断就是硬件产生的中断，另外一种是int n 产生的中断，int n产生的中断类似于中断门的调用，n 就是向量号。
   外部中断则复杂一点，需要建立向量号和硬件中断之间的关系，分为可屏蔽中断和不可屏蔽中断，由CPU的两根引脚NMI，INTR来接收。可编程中断控制器8259A来完成 ，与CPU相连的是两块8259A，分别是主从控制器，他们级联在一起。
   变成控制8259A的过程通过向特定的端口写入ICW来完成，主8259A对应的端口为20h和21h, 从8259A是A0H和A1H，初始化的过程位：

1. 往端口 20H(主片)或者A0h(从片） 写入ICW1.
1. 往端口 21H(主片)或者A1h(从片） 写入ICW2.
1. 往端口 21H(主片)或者A1h(从片） 写入ICW3.
1. 往端口 21H(主片)或者A1h(从片） 写入ICW4.

这四步的顺序是不能颠倒的。

## 几点额外说明
    实际应用中，中断的产生是带有特权等级变换的，实际上，通过终端们和陷阱门的中断相当与用call指令调用了一个调用门，实际的特权等级变换规则是一样的。

## 保护模式下的IO
  IO是需要保护的，只有运行与ring0 的指令才可以改变IOPL，也就是寄存器eflags的位，每个任务都有自己的IO许可位图。


