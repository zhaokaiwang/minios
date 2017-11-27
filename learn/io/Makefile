######################
# Makefile for minios#
######################

# Entry point of minios
# it must have the same valeu with 'kernelEntryPointPhyAddr' in load.inc
ENTRYPOINT = 0X30400

# Offset of entry point in kernel file
# It depends on ENTRYPOINT
ENTRYOFFSET = 0X400

# Programs, flags, etc.
ASM 	= nasm
DASM 	= objdump
CC 		= gcc
LD 		= ld 
ASMBFLAGS 	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32 -fno-stack-protector
LDFLAGS 	= -m elf_i386 -s -Ttext $(ENTRYPOINT) -Map krnl.map
DASMFLAGS 	= -D

# This program 
MINIOSBOOT = boot/boot.bin boot/loader.bin
MINIOSKERNEL 	= kernel.bin
OBJS			= kernel/kernel.o kernel/start.o kernel/i8259.o kernel/global.o\
			kernel/protect.o\
			kernel/main.o kernel/clock.o kernel/proc.o kernel/syscall.o \
			kernel/keyboard.o kernel/tty.o kernel/console.o kernel/printf.o \
			kernel/vsprintf.o kernel/systask.o \
			lib/assert.o lib/klib.o lib/kliba.o lib/string.o 
 
DASMOUTPUT		= kernel.bin.asm

# all phony targets
.PHONY : everything final image clean realclean disasm all buildimg

# default start position
everything: $(MINIOSBOOT) $(MINIOSKERNEL)

all : realclean everything

final: all clean

image: final buildimg

clean: 
	rm -f $(OBJS)

realclean:
	rm -f $(OBJS) $(MINIOSBOOT) $(MINIOSKERNEL)

disasm:
	$(DASM)  $(DASMFLAGS)  $(MINIOSKERNEL) >  $(DASMOUTPUT)

# we assume that "a.img" exists in current folder
buildimg:
	dd if=boot/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/floppy
	sudo cp -fv boot/loader.bin /mnt/floppy
	sudo cp -fv kernel.bin /mnt/floppy
	sudo umount /mnt/floppy

boot/boot.bin : boot/boot.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin : boot/loader.asm boot/include/load.inc \
                 boot/include/fat12hdr.inc boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(MINIOSKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(MINIOSKERNEL) $(OBJS)

kernel/kernel.o : kernel/kernel.asm  include/sconst.inc 
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/start.o : kernel/start.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259.o : kernel/i8259.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/main.o: kernel/main.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o : kernel/global.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o :  kernel/protect.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/clock.o : kernel/clock.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/proc.o : kernel/proc.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o : kernel/keyboard.c
	$(CC) $(CFLAGS) -o $@ $<
	
kernel/printf.o: kernel/printf.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/vsprintf.o: kernel/vsprintf.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o : kernel/tty.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/console.o : kernel/console.c 
	$(CC) $(CFLAGS) -o $@ $<

kernel/systask.o: kernel/systask.c
	$(CC) $(CFLAGS) -o $@ $<

lib/assert.o: lib/assert.c
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o : lib/klib.c
	$(CC) $(CFLAGS) -o $@ $<

lib/kliba.o : lib/kliba.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/string.o : lib/string.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<	

kernel/syscall.o : kernel/syscall.asm include/sconst.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<
