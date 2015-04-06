CC=gcc
ASM=nasm
LINKER=ld

CFLAGS=-Wall -O0 -std=gnu99
KFLAGS=-c -ffreestanding -m32

C_SOURCES=libkernel.c gdt.c terminal.c keyboard.c interrupt.c paging.c kheap.c proc.c syscall.c main.c
ASM_SOURCES=entry.asm gdt.asm isr.asm usermode.asm

C_OBJECTS=$(C_SOURCES:%.c=build/%.o)
ASM_OBJECTS=$(ASM_SOURCES:%.asm=build/%.ao)

ORG=0x1000

all: build/hda.img

build/bootloader.bin: src/bootloader/main.asm
	cd src/bootloader && \
	$(ASM) -f bin -o ../../build/bootloader.bin main.asm

build/%.o: src/kernel/%.c
	$(CC) $(CFLAGS) $(KFLAGS) $+ -o $@

build/%.ao: src/kernel/asm/%.asm
	$(ASM) -f elf $+ -o $@

build/kernel.bin: $(C_OBJECTS) $(ASM_OBJECTS)
	$(LINKER) -m elf_i386 -Ttext $(ORG) $+ --oformat binary -o $@
	echo $(OBJECTS)

build/hda.img: build/kernel.bin build/bootloader.bin
	cat $+ > $@
