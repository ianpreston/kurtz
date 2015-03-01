disk_error:
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    mov al, 'r'
    int 0x10
    mov al, 'r'
    int 0x10
    jmp $  

read_disk:
    mov ah, 0x2 ; 0x02 = Read Sector
    ; BIOS sets dl to the boot drive on startup
    mov ch, 0x0 ; Cylinder
    mov dh, 0x0 ; Head
    mov cl, 0x2 ; Sector
    mov al, 0x8 ; Sectors to read

    ; Interrupt to read from disk
    int 0x13

    ;jc disk_error
    ;cmp al, 0x1
    ;jne disk_error

    ret
