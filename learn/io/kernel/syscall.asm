;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                       zhaokai, 2017
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
%include "sconst.inc"

INT_VECTOR_SYS_CALL             equ     0x90
_NR_printx                      equ     0
_NR_sendrec                     equ     1

; 导出符号
global  sendrec
global  printx

bits 32
[section .text]
;===============================================================
;               sendrec(int function, int  src_dest, MESSAGE* m)
;===============================================================
sendrec:
        mov     eax, _NR_sendrec
        mov     ebx, [esp + 4]  ; function
        mov     ecx, [esp + 8]  ; src_dest
        mov     edx, [esp + 12] ; p_msg
        int     INT_VECTOR_SYS_CALL
        ret 
;==============================================================
;               void printx(char* s)
;================================================================
printx:
        mov     eax, _NR_printx
        mov     edx, [esp + 4]
        int     INT_VECTOR_SYS_CALL
        ret 