; TODO - Don't duplicate GDT descriptors here
[GLOBAL flush_gdt]
flush_gdt:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x8:post_flush_gdt
post_flush_gdt:
    ret

[GLOBAL flush_tss]
flush_tss:
    mov ax, 0x2B
    ltr ax
    ret
