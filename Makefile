CC=gcc
ASM=nasm
LINKER=ld

CFLAGS=-Wall -Werror -pedantic -O0 -std=gnu99
KFLAGS=-c -ffreestanding -m32

C_SOURCES=libkernel.c gdt.c terminal.c keyboard.c interrupt.c paging.c kheap.c proc.c syscall.c initrd.c main.c
ASM_SOURCES=multiboot.asm gdt.asm isr.asm usermode.asm

C_OBJECTS=$(C_SOURCES:%.c=build/%.o)
ASM_OBJECTS=$(ASM_SOURCES:%.asm=build/%.ao)

all: build/cdrom.iso

build/%.o: src/kernel/%.c
	$(CC) $(CFLAGS) $(KFLAGS) $+ -o $@

build/%.ao: src/kernel/asm/%.asm
	$(ASM) -f elf $+ -o $@

build/kernel.bin: $(ASM_OBJECTS) $(C_OBJECTS)
	$(LINKER) -m elf_i386 -T linker.ld $+ -o $@
	echo $(OBJECTS)

build/initrd.tar: initrd/
	tar cvf build/initrd.tar initrd/*

build/cdrom.iso: build/kernel.bin build/initrd.tar
	mkdir -p build/isoroot/boot/grub || true
	mkdir -p build/isoroot/initrd || true
	cp grub.cfg build/isoroot/boot/grub/grub.cfg
	cp build/initrd.tar build/isoroot/initrd/initrd.tar
	cp build/kernel.bin build/isoroot/boot/kernel.bin
	grub-mkrescue -o build/cdrom.iso build/isoroot
