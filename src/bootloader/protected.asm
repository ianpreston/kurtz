switch_to_protected:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_protected

[bits 32]

init_protected:
    ; Set up segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up the stack
    mov ebp, 0x90000
    mov esp, ebp
    
    call start_kernel

start_kernel:
    jmp KERNEL_OFFSET
