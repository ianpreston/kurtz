; TODO - Don't duplicate GDT descriptors here
[GLOBAL crunchatize_me_capn]
crunchatize_me_capn:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23
    push esp

    pushfd
    pop eax
    or eax, 0x200
    push eax

    push 0x1B
    lea eax, [enter_usermode_next]
    push eax
    iret

; FIXME - Move to new file. Preferrably don't link with kernel.
enter_usermode_next:
    add esp, 0x4
    mov edx, 0xDEADBEEF
    int 0xF1
    mloop:
        jmp mloop
