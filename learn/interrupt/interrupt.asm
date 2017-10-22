; 测试中断的实现，并实现一个时钟中断的小程序

%include	"pm.inc"	; 常量, 宏, 以及一些说明

org	0100h
	jmp	LABEL_BEGIN

[SECTION .gdt]
; GDT
;                                         段基址,       段界限     , 属性
LABEL_GDT:		Descriptor	       0,                 0, 0				; 空描述符
LABEL_DESC_NORMAL:	Descriptor	       0,            0ffffh, DA_DRW			; Normal 描述符
LABEL_DESC_CODE32:	Descriptor	       0,  SegCode32Len - 1, DA_CR | DA_32		; 非一致代码段, 32
LABEL_DESC_CODE16:	Descriptor	       0,            0ffffh, DA_C			; 非一致代码段, 16
LABEL_DESC_DATA:	Descriptor	       0,	DataLen - 1, DA_DRW			; Data
LABEL_DESC_STACK:	Descriptor	       0,        TopOfStack, DA_DRWA | DA_32		; Stack, 32 位
LABEL_DESC_VIDEO:	Descriptor	 0B8000h,            0ffffh, DA_DRW			; 显存首地址
; GDT 结束

GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1	; GDT界限
		dd	0		; GDT基地址

; GDT 选择子
SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT
SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT
SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
; END of [SECTION .gdt]

[SECTION .data1]	 ; 数据段
ALIGN	32
[BITS	32]
LABEL_DATA:
; 实模式下使用这些符号
; 字符串
_wSPValueInRealMode		dw	0
_SavedIDTR:			dd	0	; 用于保存 IDTR
				dd	0
_SavedIMREG:			db	0	; 中断屏蔽寄存器值
_MemChkBuf:	times	256	db	0
_dwDispPos:			dd	(80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
; 保护模式下使用这些符号
SavedIDTR		equ	_SavedIDTR	- $$
SavedIMREG		equ	_SavedIMREG	- $$
_szReturn			db	0Ah, 0
dwDispPos:			dd	(80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
szReturn           db  0ah, 0
DataLen			equ	$ - LABEL_DATA
; END of [SECTION .data1]

; 中断表段
[SECTION .idt]
ALIGN 32
[BITS 32]
LABEL_IDT:
; 门                 目标选择子，      偏移，DCOUNT， 属性
%rep 32
    Gate   SelectorCode32, SpuriousHandler, 0, DA_386IGate
%endrep 
.020h: Gate SelectorCode32, ClockHandler, 0, DA_386IGate
%rep 95
    Gate SelectorCode32, SpuriousHandler, 0, DA_386IGate
%endrep
.080h:   Gate SelectorCode32, UserIntHandler, 0, DA_386IGate

IdtLen      equ     $ - LABEL_IDT
IdtPtr      dw      IdtLen - 1; 段界限
            dd     0    ; 基地址
; END OF [SECTION .idt]

[SECTION .gs]
ALIGN 32
[BITS 32]
LABEL_STACK:
    times 512 db 0

TopOfStack     equ   $ - LABEL_STACK - 1
; END OF [SECTION .gs]

[SECTION .s16]
[BITS 16]
LABEL_BEGIN:
    mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h

	mov	[LABEL_GO_BACK_TO_REAL+3], ax
	mov	[_wSPValueInRealMode], sp
	; 初始化 16 位代码段描述符
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

	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt 基地址
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 为加载 IDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_IDT		; eax <- idt 基地址
	mov	dword [IdtPtr + 2], eax	; [IdtPtr + 2] <- idt 基地址

	; 保存 IDTR
	sidt	[_SavedIDTR]

	; 保存中断屏蔽寄存器(IMREG)值
	in	al, 21h
	mov	[_SavedIMREG], al

	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	;cli

	; 加载 IDTR
	lidt	[IdtPtr]

	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 真正进入保护模式
	jmp	dword SelectorCode32:0	; 执行这一句会把 SelectorCode32 装入 cs, 并跳转到 Code32Selector:0  处

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LABEL_REAL_ENTRY:		; 从保护模式跳回到实模式就到了这里
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, [_wSPValueInRealMode]

	lidt	[_SavedIDTR]	; 恢复 IDTR 的原值

	mov	al, [_SavedIMREG]	; ┓恢复中断屏蔽寄存器(IMREG)的原值
	out	21h, al			; ┛

	in	al, 92h		; ┓
	and	al, 11111101b	; ┣ 关闭 A20 地址线
	out	92h, al		; ┛

	sti			; 开中断

	mov	ax, 4c00h	; ┓
	int	21h		; ┛回到 DOS
; END of [SECTION .s16]

;32 位代码段， 从实模式跳转到此
[SECTION .32]
[BITS 32]

LABEL_SEG_CODE32:
    mov ax, SelectorData
    mov ds, ax
    mov es, ax
    mov ax, SelectorVideo
    mov gs, ax
    
    mov ax, SelectorStack
    mov ss, ax 
    mov esp, TopOfStack

    call Init8259A

    int 080h 
    sti 
    jmp $

; Init8259A ---------------------------------------------------
Init8259A:
    mov al, 011h
    out 020h, al ; 主 8259， ICW1
    call io_delay 
    
    out 0a0h, al ; 主 8259， ICW1
    call io_delay 

    mov al, 020h  ; IRQ0 对应中断向量 0x20
    out 021h, al ; 主 8259， ICW2
    call io_delay

    mov al, 028h  ; IRQ8 对应中断向量 0x28
    out 0a1h, al ; 从 8259， ICW2
    call io_delay 

    mov al, 004h  ;IR2 对应从 8259
    out 021h, al ; 从 8259， ICW3
    call io_delay 

    mov al, 002h  ;对应主8259的 IR2
    out 0A1h, al ; 主 8259， ICW1
    call io_delay 

    mov al, 001h 
    out 021h, al  ;主8259， ICW4
    call io_delay

    out 0a1h, al   ;从 8259， ICW4
    call io_delay
    
    ; 仅开启定时器中断
    mov al, 11111110b
    out 021h, al
    call io_delay
    
    ; 屏蔽从 8259的所有中断
    mov al, 11111111b
    out 0a1h, al
    call io_delay
    
    ret 
; end of init8259a-----------------------------------------------

; SetRealmode8259A ---------------------------------------------------------------------------------------------
SetRealmode8259A:
	mov	ax, SelectorData
	mov	fs, ax

	mov	al, 017h
	out	020h, al	; 主8259, ICW1.
	call	io_delay

	mov	al, 008h	; IRQ0 对应中断向量 0x8
	out	021h, al	; 主8259, ICW2.
	call	io_delay

	mov	al, 001h
	out	021h, al	; 主8259, ICW4.
	call	io_delay

	mov	al, [fs:SavedIMREG]	; ┓恢复中断屏蔽寄存器(IMREG)的原值
	out	021h, al		; ┛
	call	io_delay

	ret
; SetRealmode8259A ---------------------------------------------------------------------------------------------

io_delay:
    nop 
    nop 
    nop 
    nop 
    ret 

; int handler 
_ClockHandler:
    ClockHandler    equ   _ClockHandler - $$
    inc byte [gs:((80 * 0 + 20) * 2)]  ; 屏幕第 0 行， 第 70 列，改变现存中的数
    mov al, 020h
    out 020h, al            ;发送 EOI
    iretd 

_UserIntHandler:
UserIntHandler:       equ   _UserIntHandler - $$
    mov ah, 0ch
    mov al, 'I'
    mov [gs:((80 * 0 + 70) * 2)], ax 
    iretd
; -------------------------------------------------------

_SpuriousHandler:
    SpuriousHandler       equ   _SpuriousHandler - $$
    mov ah, 0ch
    mov al, '1'
    mov [gs:((80 * 2 + 75) * 2)],ax 
    jmp $
    iretd
;----------------------------------
SegCode32Len     equ     $ - LABEL_SEG_CODE32
; end of  [section .s32]

%include "lib.inc"
; 16 位代码段. 由 32 位代码段跳入, 跳出后到实模式
[SECTION .s16code]
ALIGN	32
[BITS	16]
LABEL_SEG_CODE16:
	; 跳回实模式:
	mov	ax, SelectorNormal
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	mov	eax, cr0
	and	al, 11111110b
	mov	cr0, eax

LABEL_GO_BACK_TO_REAL:
	jmp	0:LABEL_REAL_ENTRY	; 段地址会在程序开始处被设置成正确的值

Code16Len	equ	$ - LABEL_SEG_CODE16

; END of [SECTION .s16code]






