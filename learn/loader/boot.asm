;boot 和简单的loader
;%define __BOOT_DEBUG_
%ifdef __BOOT_DEBUG_
    org 0100h
%else
    org 07c00h
%endif

;==============================================
;是否调试模式下的栈的地址
%ifdef  __BOOT_DEBUG_
BaseOfStack     equ 0100h
%else
BaseOfStack     equ 07c00h
%endif 

BaseOfLoader       equ 09000h ; LOADER.BIN被加载的位置,段地址
OffsetOfLoader     equ 0100h; LOADER.BIN被加载的位置，偏移
RootDirSectors     equ 14;  根目录占用空间
SectorNoOfRootDirectory equ 19 ;ROOT directory的第一个扇区号
SectorNoOfFAT1     equ   1;FAT1 的第一个扇区号
DeltaSectorNo      equ   17 ; DeltaSectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) - 2
					; 文件的开始Sector号 = DirEntry中的开始Sector号 + 根目录占用Sector数目 + DeltaSectorNo
                    ; - 2的原因是数据区开始的簇号是2，而不是0  
;==================================================
    jmp short LABEL_START ; start to boot
    nop

    ; 下面是 FAT12 磁盘的头
	BS_OEMName	DB 'ForrestY'	; OEM String, 必须 8 个字节
	BPB_BytsPerSec	DW 512		; 每扇区字节数
	BPB_SecPerClus	DB 1		; 每簇多少扇区
	BPB_RsvdSecCnt	DW 1		; Boot 记录占用多少扇区
	BPB_NumFATs	DB 2		; 共有多少 FAT 表
	BPB_RootEntCnt	DW 224		; 根目录文件数最大值
	BPB_TotSec16	DW 2880		; 逻辑扇区总数
	BPB_Media	DB 0xF0		; 媒体描述符
	BPB_FATSz16	DW 9		; 每FAT扇区数
	BPB_SecPerTrk	DW 18		; 每磁道扇区数
	BPB_NumHeads	DW 2		; 磁头数(面数)
	BPB_HiddSec	DD 0		; 隐藏扇区数
	BPB_TotSec32	DD 0		; 如果 wTotalSectorCount 是 0 由这个值记录扇区数
	BS_DrvNum	DB 0		; 中断 13 的驱动器号
	BS_Reserved1	DB 0		; 未使用
	BS_BootSig	DB 29h		; 扩展引导标记 (29h)
	BS_VolID	DD 0		; 卷序列号
	BS_VolLab	DB 'OrangeS0.02'; 卷标, 必须 11 个字节
	BS_FileSysType	DB 'FAT12   '	; 文件系统类型, 必须 8个字节

LABEL_START:
    mov ax, cs 
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov sp, BaseOfStack

    ; 清屏
    mov ax, 0600h
    mov bx, 0700h 
    mov cx, 0
    mov dx, 0184fh
    int 10h

    mov dh, 0
    call DispStr

    ;软驱复位
    xor ah, ah
    xor dl, dl ;a盘
    int 13h

; 下面在 A 盘根目录下寻找 LOADER.BIN
    mov word [wSectorNo], SectorNoOfRootDirectory ;根目录的扇区号
LABEl_SEARCH_IN_ROOT_DIR_BEGIN:
    cmp word [wRootDirSizeForLoop], 0 ;判断根目录是不是没有LOADERBIN
    jz LABEL_NO_LOADERBIN       ;没有找到 LOADERBIN
    dec word [wRootDirSizeForLoop]
    mov ax, BaseOfLoader   
    mov es, ax          ; es <- BaseOfLoader
    mov bx, OffsetOfLoader ; es:bx BaseOfLoader:OffsetOfLoader
    mov ax, [wSectorNo]  
    mov cl,1
    call ReadSector     ;将开始扇区为 AX， 个数位 cl个扇区的内容
                        ;填入到 es:bx的地址处

    mov si, LoaderFileName
    mov di, OffsetOfLoader
    cld 
    mov dx, 10h
LABEL_SEARCH_FOR_LOADERBIN:
    cmp dx, 0           ; 循环次数控制
    jz  LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR; 如果读完了就去下一个申请扇区
    dec dx
    mov cx, 11           ; 因为在这个文件系统中，文件名8位，扩展名3位
LABEL_CMP_FILENAME:
    cmp cx, 0               
    jz  LABEL_FILENAME_FOUND
    dec cx
    lodsb
    cmp al, byte [es:di]
    jz LABEL_GO_ON 
    jmp LABEL_DIFFERENT

LABEL_GO_ON:
    inc di
    jmp LABEL_CMP_FILENAME

LABEL_DIFFERENT:
    and di, 0ffe0h  ;因为每一个根目录的大小位 32位，所以我们将后5位清零
    add di, 20h     ;然后加 0x20，指向下一个目录项
    mov si, LoaderFileName
    jmp LABEL_SEARCH_FOR_LOADERBIN

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
    add word [wSectorNo],1
    jmp LABEl_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_LOADERBIN:
    mov dh, 2
    call DispStr
%ifdef	_BOOT_DEBUG_
	mov	ax, 4c00h		; `.
	int	21h			; /  没有找到 LOADER.BIN, 回到 DOS
%else
	jmp	$			; 没有找到 LOADER.BIN, 死循环在这里
%endif

;es:di 指向对应文件名的目录项，我们根据里面的内容来找到文件
LABEL_FILENAME_FOUND:			; 找到 LOADER.BIN 后便来到这里继续
	mov ax, RootDirSectors			; 
    and di, 0ffe0h                  ;目录项的七点
    add di, 01ah                    ;簇号,在我们这里其实也就是扇区号
    mov cx, word [es:di]            ;保存FAT的序号
    push cx 
    add cx, ax 
    add cx, DeltaSectorNo
    mov ax, BaseOfLoader       
    mov es, ax                      ;在这里我们还是直接把内容读到了 baseoflader:offset的地方         
    mov bx, OffsetOfLoader
    mov ax, cx                      ;ax 是要读的扇区号

LABEL_GOON_LOADING_FILE:
    push ax             
    push bx 
    mov ah, 0eh            
    mov al, '.'             ;每读一个扇区就在后面加一个 '.'
    mov bl, 0fh 
    int 10h
    pop bx
    pop ax 
    
    mov cl,1                 ;读文件内容的一个扇区
    call ReadSector
    pop ax                   ;取出其中的FAT号
    call GetFATEntry
    cmp ax, 0fffh             ;此处如果内容大于0FFFH，则表明已经是最后一个扇区了
    jz LABEL_FILE_LOADED
    push ax 
    mov dx, RootDirSectors
    add ax, dx 
    add ax, DeltaSectorNo
    add bx, [BPB_BytsPerSec]
    jmp LABEL_GOON_LOADING_FILE

LABEL_FILE_LOADED:
    mov dh, 1
    call DispStr

    jmp BaseOfLoader:OffsetOfLoader ; 这一句跳转到已经加载的内核，开始执行代码


;============================================================================
;变量
wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数，
						; 在循环中会递减至零.
wSectorNo		dw	0		; 要读取的扇区号
bOdd			db	0		; 奇数还是偶数

;字符串
LoaderFileName		db	"LOADER  BIN", 0 ; LOADER.BIN 之文件名
; 为简化代码, 下面每个字符串的长度均为 MessageLength
MessageLength		equ	9
BootMessage:		db	"Booting  " ; 9字节, 不够则用空格补齐. 序号 0
Message1		db	"Ready.   " ; 9字节, 不够则用空格补齐. 序号 1
Message2		db	"No LOADER" ; 9字节, 不够则用空格补齐. 序号 2
;============================================================================
;----------------------------------------------------------------------------
; 函数名: DispStr
;----------------------------------------------------------------------------
; 作用:
;	显示一个字符串, 函数开始时 dh 中应该是字符串序号(0-based)
DispStr:
	mov	ax, MessageLength
	mul	dh
	add	ax, BootMessage
	mov	bp, ax			; `.
	mov	ax, ds			;  | ES:BP = 串地址
	mov	es, ax			; /
	mov	cx, MessageLength	; CX = 串长度
	mov	ax, 01301h		; AH = 13,  AL = 01h
	mov	bx, 0007h		; 页号为0(BH = 0) 黑底白字(BL = 07h)
	mov	dl, 0
	int	10h			; int 10h
	ret
;------------------------------------------
; 从第 AX 个sector 开始，将 cl 个 sector 读入 ES:BX 中
ReadSector:
    ;-----------------------
    ;设扇区号为 X

    ;X / 每磁道扇区数 商 y 柱面号 Y>>1 磁头号 y & 1
    ; 余 z 其实扇区号 = Z + 1
    push bp  
    mov bp, sp
    sub esp, 2

    mov byte [bp - 2], cl
    push bx
    mov bl, [BPB_SecPerTrk]; 每磁道扇区数
    div bl          ;Y在al中， z在AH中
    inc ah
    mov cl, ah ; cl 起始扇区
    mov dh, al 
    shr al,1
    mov ch, al  ;柱面号
    and dh, 1  ;磁头号

    pop bx      ;恢复 BX
    mov dl, [BS_DrvNum]
.GoOnReading:
    mov ah, 2           ;读数据
    mov al, byte[bp - 2]
    int 13h
    jc .GoOnReading     ;如果读取错误 CF 会被置位1，这时不停的读，知道正确为止

    add esp, 2
    pop bp
    ret 

;------------------------------------------------
;GETTATENTRY
; 找到 序号为ax 的 sector 在 FAT中的条目， 结果放在AX 中
; 因为 es:bx中保存的内容是文件已经读取到内存的部分，所以我们在这里
; 用栈来保存它们
GetFATEntry:
    push es
    push bx 
    push ax 
    mov ax ,BaseOfLoader
    sub ax , 0100h
    mov es, ax  ;在 BASEOFLOADER中保留 0100H 的空间 
    pop ax 
    mov byte [bOdd], 0
    mov bx, 3            ;因为每个12位，所以 * 3 / 2为一字节
    mul bx              ;dx:ax = ax * 3 
    mov bx, 2
    div bx              ; dx:ax / 2 ==>ax 商 dx 余数
    cmp dx, 0
    jz LABEL_EVEN
    mov byte [bOdd], 1

LABEL_EVEN:
    xor dx, dx 
    mov bx, [BPB_BytsPerSec] ;扇区数
    div bx                  ;此处计算在那个扇区和偏移量
                            ; ax 商，扇区
                            ; dx 余数，扇区的偏移量
    push dx                 
    mov bx, 0
    add ax, SectorNoOfFAT1
    mov cl, 2               ;此处是害怕处于连个扇区的中间
    call ReadSector
    pop dx                  ;dx 是偏移量
    add bx, dx 
    mov ax, [es:bx]
    ; ax 是以目标偏移量为目标的16位数，所以这里区分奇偶
    cmp byte [bOdd], 1
    jnz LABEL_EVEN_2
    shr ax, 4

LABEL_EVEN_2:
    and ax, 0fffh

LABEL_GET_FAT_ENRY_OK:
    pop bx
    pop es 
    ret
times 510-($-$$) db 0
dw 0xaa55;结束标志


