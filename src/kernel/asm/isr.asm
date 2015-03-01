[bits 32]
[EXTERN kernel_isr]
[EXTERN kernel_irq]

%macro ISR_NOCODE 1
    [GLOBAL isr_%1]
    isr_%1:
        cli 
        push byte 0
        push byte %1
        jmp interrupt_handler
%endmacro

%macro ISR_CODE 1
    [GLOBAL isr_%1]
    isr_%1:
        cli 
        push byte %1
        jmp interrupt_handler
%endmacro

%macro IRQ 2
    [GLOBAL irq_%1]
    irq_%1:
        cli
        push byte 0
        push byte %2
        jmp irq_handler
%endmacro

; x86 Exceptions
ISR_NOCODE 0
ISR_NOCODE 1
ISR_NOCODE 2
ISR_NOCODE 3
ISR_NOCODE 4
ISR_NOCODE 5
ISR_NOCODE 6
ISR_NOCODE 7
ISR_CODE 8
ISR_NOCODE 9
ISR_CODE 10
ISR_CODE 11
ISR_CODE 12
ISR_CODE 13
ISR_CODE 14
ISR_NOCODE 15
ISR_NOCODE 16
ISR_NOCODE 17
ISR_NOCODE 18

; Kurtz syscall
ISR_NOCODE 241

; IRQ Handlers
IRQ 0, 32
IRQ 1, 33
IRQ 15, 40

interrupt_handler:
    pusha
    call kernel_isr
    popa
    add esp, 8
    sti
    iret

irq_handler:
    pusha
    call kernel_irq
    popa
    add esp, 8
    sti
    iret
