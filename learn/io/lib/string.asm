; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                              string.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                      zhaokai, 2017
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
[SECTION .text]

; 导出函数
global	memcpy
global 	memset
global  strcpy
global  strlen
;------------------------
; void* memcpy(void* es:pDest, void* ds:pSrc, int iSize )
;------------------------
memcpy:
	push ebp
	mov ebp, esp

	push esi
	push edi
	push ecx

	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ecx, [ebp + 16]
.1:
	cmp	ecx, 0
	jz .2    ;完成

	mov al,[ds:esi]
	inc esi

	mov byte [es:edi], al
	inc edi

	dec ecx
	jmp .1
.2:
	mov eax, [ebp + 8]  ; return value
	pop ecx
	pop edi
	pop esi
	mov esp, ebp
	pop ebp

	ret 
; memcpy end 


; ------------------------------------
; memset(void* p_desc, char ch, int size)
;---------------------------------------
memset:
	push ebp
	mov ebp, esp

	push esi 
	push edi
	push ecx 

	mov edi, [ebp + 8]
	mov edx, [ebp + 12]
	mov ecx, [ebp + 16]

.1:
	cmp	ecx, 0
	jz	.2
	mov byte [edi], dl
	inc edi

	dec ecx
	jmp .1
.2:
	pop ecx 
	pop edi
	pop esi
	mov esp, ebp
	pop ebp

	ret 

;----------------------------------------------
;char* strcpy(char* p_dest, char* p_src)
;---------------------------------------------
strcpy:
	push ebp
	mov  ebp, esp 

	mov esi, [ebp + 12]
	mov edi, [ebp + 8]

.1:
	mov al, [esi]
	inc esi 

	mov byte[edi], al
	inc edi

	cmp al, 0
	jnz .1

	mov eax, [ebp + 18]
	pop ebp 
	ret 
; end of strcpy
; ------------------------------------------------------------------------
; int strlen(char* p_str);
; ------------------------------------------------------------------------
strlen:
        push    ebp
        mov     ebp, esp

        mov     eax, 0                  ; 字符串长度开始是 0
        mov     esi, [ebp + 8]          ; esi 指向首地址

.1:
        cmp     byte [esi], 0           ; 看 esi 指向的字符是否是 '\0'
        jz      .2                      ; 如果是 '\0'，程序结束
        inc     esi                     ; 如果不是 '\0'，esi 指向下一个字符
        inc     eax                     ;         并且，eax 自加一
        jmp     .1                      ; 如此循环

.2:
        pop     ebp
        ret                             ; 函数结束，返回
; -----------------------------------------------------------------