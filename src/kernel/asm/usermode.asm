; TODO - Don't duplicate GDT descriptors here
[GLOBAL crunchatize_me_capn]
crunchatize_me_capn:
    cli

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    pop edx
    pop edx ; Binary base
    pop eax ; Stack base
    add esp, 0x4

    push dword 0x23
    push dword eax

    pushfd
    or dword[esp], 0x200 ; Set interrupt bit of EFLAGS

    push dword 0x1B
    push dword edx
    iret
