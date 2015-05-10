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
resb 16384
stack_top:


section .text
extern main
global _start
_start:
    mov esp, stack_top
    mov ebp, stack_base

    push ebx
    push eax
    call main 

    cli
    chill: jmp chill
