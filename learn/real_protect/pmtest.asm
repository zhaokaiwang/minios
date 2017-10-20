%include "pm.inc"
org 0100h
    jmp LABEL_BEGIN
[SECTION .gdt]
;
LABEL_GDT:             Descriptor 0,                 0, 0		   ;空描述符
LABEL_DESC_NORMAL:     Descriptor 0,            0ffffh, DA_DRW		   ;Normal描述符
LABEL_DESC_CODE32:     Descriptor 0,    SegCode32Len-1, DA_C+DA_32	   ;非一致,32
LABEL_DESC_CODE16:     Descriptor 0,            0ffffh, DA_C		   ;非一致,16
LABEL_DESC_CODE_DEST:  Descriptor 0,  SegCodeDestLen-1, DA_C+DA_32	   ;非一致,32
LABEL_DESC_CODE_RING3: Descriptor 0,  SegCodeRing3Len-1, DA_C+DA_32+DA_DPL3
LABEL_DESC_DATA:       Descriptor 0,	     DataLen-1, DA_DRW             ;Data
LABEL_DESC_STACK:      Descriptor 0,        TopOfStack, DA_DRWA+DA_32	   ;Stack,32
LABEL_DESC_STACK3:     Descriptor 0,       TopOfStack3, DA_DRWA+DA_32+DA_DPL3
LABEL_DESC_LDT:        Descriptor 0,          LDTLen-1, DA_LDT		   ;LDT
LABEL_DESC_TSS:        Descriptor 0,          TSSLen-1, DA_386TSS	   ;TSS
LABEL_DESC_VIDEO:      Descriptor 0B8000h,      0ffffh, DA_DRW+DA_DPL3

;GATE
LABEL_CALL_GATE_TEST: Gate SelectorCodeDest,   0,  0, DA_386CGate + DA_DPL3

GdtLen          equ  $ - LABEL_GDT
GdtPtr          dw   GdtLen - 1

;GDT 选择子
SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT
SelectorCodeDest	equ	LABEL_DESC_CODE_DEST	- LABEL_GDT
SelectorCodeRing3	equ	LABEL_DESC_CODE_RING3	- LABEL_GDT + SA_RPL3
SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
SelectorStack3		equ	LABEL_DESC_STACK3	- LABEL_GDT + SA_RPL3
SelectorLDT		equ	LABEL_DESC_LDT		- LABEL_GDT
SelectorTSS		equ	LABEL_DESC_TSS		- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT

SelectorCallGateTest   equ  LABEL_CALL_GATE_TEST  - LABEL_GDT + SA_RPL3
;end of [SECTION .gdt]

[SECTION .data1]
ALIGN   32
[BITS   32]
LABEL_DATA:
SPValueInRealMode       dw  0
;
PMMessage:      db  "Now in protect mode", 0
OffsetPMMessage     equ  PMMessage - $$
Strtest:        db  "ABCDEFG", 0
OffsetStrtest       equ Strtest - $$
DataLen             equ $ - LABEL_DATA
;end of [section .data]

;全局堆栈段
[SECTION .gs]
ALIGN   32
[BITS   32]
LABEL_STACK:
    times 512 db 0
TopOfStack  equ $ - LABEL_STACK - 1
;END OF [SECTION .gs]

; 堆栈段ring3
[SECTION .s3]
ALIGN	32
[BITS	32]
LABEL_STACK3:
	times 512 db 0
TopOfStack3	equ	$ - LABEL_STACK3 - 1
; END of [SECTION .s3]

; TSS ---------------------------------------------------------------------------------------------
[SECTION .tss]
ALIGN	32
[BITS	32]
LABEL_TSS:
		DD	0			; Back
		DD	TopOfStack		; 0 级堆栈
		DD	SelectorStack		; 
		DD	0			; 1 级堆栈
		DD	0			; 
		DD	0			; 2 级堆栈
		DD	0			; 
		DD	0			; CR3
		DD	0			; EIP
		DD	0			; EFLAGS
		DD	0			; EAX
		DD	0			; ECX
		DD	0			; EDX
		DD	0			; EBX
		DD	0			; ESP
		DD	0			; EBP
		DD	0			; ESI
		DD	0			; EDI
		DD	0			; ES
		DD	0			; CS
		DD	0			; SS
		DD	0			; DS
		DD	0			; FS
		DD	0			; GS
		DD	0			; LDT
		DW	0			; 调试陷阱标志
		DW	$ - LABEL_TSS + 2	; I/O位图基址
		DB	0ffh			; I/O位图结束标志
TSSLen		equ	$ - LABEL_TSS
; TSS ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

[SECTION .s16]
[BITS   16]
LABEL_BEGIN:
    mov ax, cs 
    mov ds, ax 
    mov es, ax
    mov ss, ax
    mov sp, 0100h

    mov [LABEL_GO_BACK_TO_REAL + 3], ax
    mov [SPValueInRealMode], sp
    
    ;初始化16位代码段描述号
	mov	ax, cs
	movzx	eax, ax
	shl	eax, 4
	add	eax, LABEL_SEG_CODE16
	mov	word [LABEL_DESC_CODE16 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE16 + 4], al
	mov	byte [LABEL_DESC_CODE16 + 7], ah

	; 初始化 32 位代码段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE32
	mov	word [LABEL_DESC_CODE32 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE32 + 4], al
	mov	byte [LABEL_DESC_CODE32 + 7], ah

	; 初始化测试调用门的代码段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE_DEST
	mov	word [LABEL_DESC_CODE_DEST + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE_DEST + 4], al
	mov	byte [LABEL_DESC_CODE_DEST + 7], ah

	; 初始化数据段描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_DATA
	mov	word [LABEL_DESC_DATA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_DATA + 4], al
	mov	byte [LABEL_DESC_DATA + 7], ah

	; 初始化堆栈段描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_STACK
	mov	word [LABEL_DESC_STACK + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK + 4], al
	mov	byte [LABEL_DESC_STACK + 7], ah

	; 初始化堆栈段描述符(ring3)
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_STACK3
	mov	word [LABEL_DESC_STACK3 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK3 + 4], al
	mov	byte [LABEL_DESC_STACK3 + 7], ah

	; 初始化 LDT 在 GDT 中的描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_LDT
	mov	word [LABEL_DESC_LDT + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_LDT + 4], al
	mov	byte [LABEL_DESC_LDT + 7], ah

	; 初始化 LDT 中的描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_CODE_A
	mov	word [LABEL_LDT_DESC_CODEA + 2], ax
	shr	eax, 16
	mov	byte [LABEL_LDT_DESC_CODEA + 4], al
	mov	byte [LABEL_LDT_DESC_CODEA + 7], ah

	; 初始化Ring3描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_CODE_RING3
	mov	word [LABEL_DESC_CODE_RING3 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE_RING3 + 4], al
	mov	byte [LABEL_DESC_CODE_RING3 + 7], ah

	; 初始化 TSS 描述符
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_TSS
	mov	word [LABEL_DESC_TSS + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_TSS + 4], al
	mov	byte [LABEL_DESC_TSS + 7], ah

	;为加载 GDTR做准备
	xor	eax, eax
	mov ax, ds
	shl eax, 4
	add eax, LABEL_GDT
	mov dword [GdtPtr + 2], eax 
	lgdt 	[GdtPtr]

	cli

	in al,92h
	or al, 00000010b
	out 92h, al

	; 准备切换到保护模式
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	; 真正进入保护模式
	jmp dword SelectorCode32:0

[SECTION .s32]
[BITS 	32]
LABEL_SEG_CODE32:
	mov ax, SelectorData
	mov ds, ax 
	mov ax, SelectorVideo
	mov gs, ax 
	
	mov ax, SelectorStack
	mov ss, ax
	
	mov esp, TopOfStack
	
	;下面显示一个字符串
	mov ah, 0ch
	xor esi, esi
	xor edi, edi
	mov esi, OffsetPMMessage	
	mov edi, (80 * 10 + 0) * 2
	cld 
.1:
	lodsb
	test al, al
	jz .2
	mov [gs:edi], ax  
	add edi, 2
	jmp .1
.2:
	call DispReturn
	;load tss

	mov ax, SelectorTSS
	ltr ax ;在任务发生特权级变换的时候要切换堆栈，而内存堆栈的指针放在当前任务的tss中，所以要设置任务状态寄存器TR

   ;在这里我门分别吧，ss,esp,cs,eip push 到栈中，通过retf指令，进入到ring3，当然我们需要先准备好tss
	push	SelectorStack3
	push	TopOfStack3
	push	SelectorCodeRing3
	push	0
	retf

;dispreturn 模拟了一个回车符号
DispReturn:
	push	eax
	push 	ebx 
	mov eax, edi
	mov bl, 160
	div bl
	and eax, 0ffh
	inc eax  
	mov bl, 160
	mul bl 
	mov edi, eax 
	pop ebx
	pop eax

	ret
;disp return 
SegCode32Len equ $ - LABEL_SEG_CODE32
;end of [SECTION .S32]

[SECTION .sdest]
[BITS 32]
LABEL_SEG_CODE_DEST:
	mov ax, SelectorVideo
	mov gs, ax
	
	mov edi, (80 * 12 + 0) * 2 ;屏幕第12行， 第 0 列
	mov ah, 0ch
	mov al, 'C'
	mov	[gs:edi], ax

	mov ax, SelectorLDT
	lldt ax

	jmp SelectorLDTCodeA:0;  跳入局部任务

SegCodeDestLen	 equ $ - LABEL_SEG_CODE_DEST
;end of [section .sdest]

[SECTION .s16code]
ALIGN 32 
[BITS 16]
LABEL_SEG_CODE16:
	;跳回实模式
	mov ax, SelectorNormal
	mov  ds, ax
	mov  gs, ax
	mov  fs, ax
	mov  es, ax
	mov  ss, ax

	mov eax, cr0
	and al, 11111110b
	mov cr0, eax

LABEL_GO_BACK_TO_REAL:
	jmp 0:LABEL_REAL_ENTRY

Code16Len	equ  $ - LABEL_SEG_CODE16
;end of [section .s16code]

LABEL_REAL_ENTRY:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov sp, [SPValueInRealMode]

	in al, 92h
	and al, 11111110b
	out 92h, al
	sti 
	mov ax, 4c00h
	int 21h
;end of [section .s16]

[SECTION .ldt]
ALIGN 32
[BITS 32]
LABEL_LDT:
LABEL_LDT_DESC_CODEA  Descriptor		0, CodeALen - 1, DA_C + DA_32

LDTLen		equ $ - LABEL_LDT

;LDT 选择子
SelectorLDTCodeA	equ	LABEL_LDT_DESC_CODEA - LABEL_LDT + SA_TIL

;LDT codeA
[SECTION .la]
ALIGN 32
[BITS 32]
LABEL_CODE_A:
	mov	ax, SelectorVideo
	mov gs, ax
	
	mov edi, (80 * 13 + 0) * 2
	mov ah, 0ch
	mov al, 'l'
	mov [gs:edi], ax

	;准备由16位代码跳回实模式
	jmp  SelectorCode16:0
CodeALen	equ $ - LABEL_CODE_A
;end of [section .la]

[SECTION .ring3]
ALIGN 32
[BITS 32]
LABEL_CODE_RING3:
	mov	ax, SelectorVideo
	mov gs, ax
	
	mov edi, (80 * 14 + 0) * 2
	mov ah, 0ch
	mov al, '3'
	mov [gs:edi], ax

	call SelectorCallGateTest:0
	jmp $
SegCodeRing3Len		equ $ - LABEL_CODE_RING3
;end of [section .ring3]
		




