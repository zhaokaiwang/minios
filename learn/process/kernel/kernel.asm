; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               kernel.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     zhaokai, 2017
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

; ----------------------------------------------------------------------
; 编译连接方法:
; $ rm -f kernel.bin
; $ nasm -f elf -o kernel.o kernel.asm
; $ nasm -f elf -o string.o string.asm
; $ nasm -f elf -o klib.o klib.asm
; $ gcc -c -o start.o start.c
; $ ld -s -Ttext 0x30400 -o kernel.bin kernel.o string.o start.o klib.o
; $ rm -f kernel.o string.o start.o
; $ 
; ----------------------------------------------------------------------
%include "sconst.inc"

; 导入函数和全局变量
extern  cstart
extern 	kernel_main
extern 	exception_handler
extern 	spurious_irq
extern 	disp_str
extern 	delay

extern gdt_ptr
extern idt_ptr
extern disp_pos
extern tss 
extern p_proc_ready
extern k_reenter
extern clock_handler
extern irq_table
extern sys_call_table

bits 32
[SECTION .bss]
StackSpace      resb 2 * 1024
StackTop:   

[SECTION .data]
clock_int_msg	db    "^", 0


[section .text]

global  _start

global  restart 

global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15

global	sys_call
; kernel.bin 从这里开始
; 我们在这里需要做的就是 把loader的内容保存到我们在内核里面的内存中
; 这个命令通过sgdt来完成， 然后我们通过新的 gdt load 出来
; cstart 完成复制的这个工作
_start:             
    mov esp, StackTop
    
    mov dword [disp_pos], 0
    sgdt    [gdt_ptr]
    call cstart
    lgdt    [gdt_ptr]

    lidt [idt_ptr]
    jmp SELECTOR_KERNEL_CS:csinit

csinit:
    
    xor eax, eax 
    mov ax, SELECTOR_TSS
    ltr ax 
    jmp kernel_main

; 中断和异常 -- 硬件中断
; --------------------------------------------------
%macro hwint_master  1
	call	save     ; 保存上下文和返回地址

	; 关闭当前中断
	in	al, INT_M_CTLMASK	
	or 	al, (1 << %1)		
	out	INT_M_CTLMASK, al  

	mov 	al, EOI
	out	INT_M_CTLMASK, al 
	sti   			; 重新打开中断，因为响应中断的过程会关闭中断

        push 	%1
        call 	[irq_table + 4 * %1]		;前往中断处理程序
	pop	ecx 
	cli 

	; 重新接受当前中断
	in 	al, INT_M_CTLMASK
	and	al, ~(1 << %1)
	out	INT_M_CTLMASK, al 
	ret 
%endmacro
; -------------------------------------------

ALIGN   16
hwint00:                ; Interrupt routine for irq 0 (the clock).
	call 	save

	in 	al, INT_M_CTLMASK
	or 	al, 1
	out 	INT_M_CTLMASK, al

	mov 	al, EOI
	out	INT_M_CTL, al 
	
	sti 
	push 	0
	call 	clock_handler
	add	esp, 4
	cli	
	
	in	al, INT_M_CTLMASK
	and	al, 0xfe 
	out	INT_M_CTLMASK, al 

	ret 

	mov 	al, EOI
	out 	INT_M_CTL, al

	inc 	dword [k_reenter]
	cmp	dword [k_reenter], 0	; 判断是否是重入的中断
	jne	.1

	mov 	esp, StackTop	; 切换内核栈
	push 	restart
	jmp .2
.1:
	push 	restart_reenter	; 重入的中断
.2:	
	sti 

	push 	0
	call 	clock_handler
	add 	esp, 4
	cli 
	ret 

hwint01:                ; Interrupt routine for irq 1 (keyboard)
        hwint_master    1

ALIGN   16
hwint02:                ; Interrupt routine for irq 2 (cascade!)
        hwint_master    2

ALIGN   16
hwint03:                ; Interrupt routine for irq 3 (second serial)
        hwint_master    3

ALIGN   16
hwint04:                ; Interrupt routine for irq 4 (first serial)
        hwint_master    4

ALIGN   16
hwint05:                ; Interrupt routine for irq 5 (XT winchester)
        hwint_master    5

ALIGN   16
hwint06:                ; Interrupt routine for irq 6 (floppy)
        hwint_master    6

ALIGN   16
hwint07:                ; Interrupt routine for irq 7 (printer)
        hwint_master    7

; ---------------------------------
%macro  hwint_slave     1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN   16
hwint08:                ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8

ALIGN   16
hwint09:                ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9

ALIGN   16
hwint10:                ; Interrupt routine for irq 10
        hwint_slave     10

ALIGN   16
hwint11:                ; Interrupt routine for irq 11
        hwint_slave     11

ALIGN   16
hwint12:                ; Interrupt routine for irq 12
        hwint_slave     12

ALIGN   16
hwint13:                ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13

ALIGN   16
hwint14:                ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14

ALIGN   16
hwint15:                ; Interrupt routine for irq 15
        hwint_slave     15

; 中断和异常 -- 异常
divide_error:
	push	0xFFFFFFFF	; no err code
	push	0		; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xFFFFFFFF	; no err code
	push	1		; vector_no	= 1
	jmp	exception
nmi:
	push	0xFFFFFFFF	; no err code
	push	2		; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	0xFFFFFFFF	; no err code
	push	3		; vector_no	= 3
	jmp	exception
overflow:
	push	0xFFFFFFFF	; no err code
	push	4		; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xFFFFFFFF	; no err code
	push	5		; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xFFFFFFFF	; no err code
	push	6		; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xFFFFFFFF	; no err code
	push	7		; vector_no	= 7
	jmp	exception
double_fault:
	push	8		; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xFFFFFFFF	; no err code
	push	9		; vector_no	= 9
	jmp	exception
inval_tss:
	push	10		; vector_no	= A
	jmp	exception
segment_not_present:
	push	11		; vector_no	= B
	jmp	exception
stack_exception:
	push	12		; vector_no	= C
	jmp	exception
general_protection:
	push	13		; vector_no	= D
	jmp	exception
page_fault:
	push	14		; vector_no	= E
	jmp	exception
copr_error:
	push	0xFFFFFFFF	; no err code
	push	16		; vector_no	= 10h
	jmp	exception

exception:
	call	exception_handler
	add	esp, 4*2	; 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS
	hlt



restart:
	mov esp, [p_proc_ready]	; 首先把ESP的值对应到process的 stackframe 这个地方，这个是要恢复的进程的地址
	lldt 	[esp + P_LDT_SEL]
	lea	eax, [esp + P_STACKTOP]
	mov 	dword [tss + TSS3_S_SP0], eax
restart_reenter:
	dec 	dword[k_reenter]
	pop 	gs 
	pop 	fs 
	pop 	es 
	pop 	ds 
	popad 

	add 	esp,4
	iretd	

; save 函数保存寄存器的值
; 不使用ret 指令的原因为我们的程序的栈可能已经切换到了内核栈，我们不能使用内核栈来返回
; 所以我们保存返回地址，然后跳转到那里
save: 	
	pushad 		; 通用寄存器的值
	; 保存段寄存器的值
	push	ds 
	push 	es 
	push	fs 
	push 	gs

	; 设置段寄存器的值
	mov 	dx, ss
	mov 	ds, dx 
	mov 	es, dx 
	
	mov 	esi, esp 		;esi = 进程表起始地址

	inc 	dword [k_reenter]
	cmp	dword [k_reenter], 0
	jne		.1
	mov 	esp, StackTop		;切换内核栈
	push 	restart
	jmp		[esi + RETADR - P_STACKBASE]
.1:					; 已经在内核栈了，不需要切换
	push	restart_reenter
	jmp		[esi + RETADR - P_STACKBASE]


sys_call:
		call save

		sti 
		call 	[sys_call_table + eax * 4]
		mov 	[esi + EAXREG - P_STACKBASE], eax 
		
		cli 
		ret 
 