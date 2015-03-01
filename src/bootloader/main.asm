[bits 16]
[org 0x7C00]

jmp boot

%include "disk.asm"

KERNEL_OFFSET equ 0x1000
boot:
    ;Set up the stack
    mov bp, 0x7C00
    mov sp, bp

    ; Load the next N sectors into memory at
    ; ES:BX (0x0:KERNEL_OFFSET)
    mov bx, KERNEL_OFFSET
    call read_disk

    ; Switch to 32-bit protected mode
    call switch_to_protected

%include "gdt.asm"
%include "protected.asm"

; Pad to 512 bytes including the BIOS magic number (sector 1 must be 512 bytes)
times 510-($-$$) db 0
dw 0xAA55
