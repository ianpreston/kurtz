MBALIGN equ 1<<0
MEMINFO equ 1<<0
FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)


section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM


section .stack, nobits
align 4
stack_base:
times 16384 resb 0
stack_top:


section .text
extern main
global _start
_start:
    call main 

    chill: jmp chill
