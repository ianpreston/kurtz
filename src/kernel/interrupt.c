#include "libkernel.h"
#include "interrupt.h"
#include "terminal.h"
#include "keyboard.h"


// Specified in asm/isr.asm
extern void isr_0(uint32_t);
extern void isr_1(uint32_t);
extern void isr_2(uint32_t);
extern void isr_3(uint32_t);
extern void isr_4(uint32_t);
extern void isr_5(uint32_t);
extern void isr_6(uint32_t);
extern void isr_7(uint32_t);
extern void isr_8(uint32_t);
extern void isr_9(uint32_t);
extern void isr_10(uint32_t);
extern void isr_11(uint32_t);
extern void isr_12(uint32_t);
extern void isr_13(uint32_t);
extern void isr_14(uint32_t);
extern void isr_15(uint32_t);
extern void isr_16(uint32_t);
extern void isr_17(uint32_t);
extern void isr_18(uint32_t);
extern void isr_241(uint32_t);
extern void irq_0(uint32_t);
extern void irq_1(uint32_t);
extern void irq_15(uint32_t);

static idt_descriptor interrupt_table[256];
static idt_ptr interrupt_ptr;

void kernel_isr(registers_t r)
{
    print("Handled interrupt "); 
    print_hex((uint8_t)r.int_no);
    print("\n");
}

void kernel_irq(registers_t r)
{
    if (r.int_no == 0x21)
    {
        // FIXME - Reverse dependencies here; interrupt handler shouldn't
        // have knowledge of drivers.
        handle_keyboard_irq();
    }

    // Master PIC handles IRQ 0-7 (ISR 32-40), slave handles
    // 40-47. Reset the master PIC, and if this is from the
    // slave PIC, reset the slave as well.
    if (r.int_no >= 40)
    {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

static void set_idt_entry(int interrupt_num, uint32_t addr)
{
    interrupt_table[interrupt_num] = (idt_descriptor){
        .selector = 0x8,
        .zero = 0x0,
        .attributes = 0x8E,
        .offset_low = addr & 0xFFFF,
        .offset_high = (addr >> 16) & 0xFFFF,
    };
}

static inline void lidt(uint32_t ptr)
{
    asm volatile ("lidt (%0)" : "=p" (ptr));
}

void init_interrupts()
{
    reset_pic();
    init_idt();

    asm volatile ("sti");
    print_sys("Enabled interrupts\n");
}

void reset_pic()
{
    outb(0x20, 0x11); // 0x11 = Start initialization
    outb(0xA0, 0x11); //

    outb(0x21, 0x20); // Specify IRQ offset; 0x20 means IRQ 0 = ISR 32, etc
    outb(0xA1, 0x28); //

    outb(0x21, 0x04); // Tell the master PIC where the slave is (0b00000100)
    outb(0xA1, 0x02); // Tell the slave which PIC it is (0b000000010)

    outb(0x21, 0x01); // Specify 8086 mode (as opposed to what, I have no idea)
    outb(0xA1, 0x01); //

    outb(0x21, 0x00); // Set the PIC's 8-bit mask; each bit represents one of
    outb(0xA1, 0x00); // the pins on the PIC. High = ignore IRQ, low = handle IRQ.
}

void init_idt()
{
    // Set all IDT entries to a dummy handler initially, then override certain
    // entries with specific ISRs.
    for (int i=0; i<256; i++)
    {
        set_idt_entry(i, (uint32_t)isr_241);
    }

    // x86 Exceptions
    set_idt_entry(0x0, (uint32_t)isr_0);
    set_idt_entry(0x1, (uint32_t)isr_1);
    set_idt_entry(0x2, (uint32_t)isr_2);
    set_idt_entry(0x3, (uint32_t)isr_3);
    set_idt_entry(0x4, (uint32_t)isr_4);
    set_idt_entry(0x5, (uint32_t)isr_5);
    set_idt_entry(0x6, (uint32_t)isr_6);
    set_idt_entry(0x7, (uint32_t)isr_7);
    set_idt_entry(0x8, (uint32_t)isr_8);
    set_idt_entry(0x9, (uint32_t)isr_9);
    set_idt_entry(0xA, (uint32_t)isr_10);
    set_idt_entry(0xB, (uint32_t)isr_11);
    set_idt_entry(0xC, (uint32_t)isr_12);
    set_idt_entry(0xD, (uint32_t)isr_13);
    set_idt_entry(0xE, (uint32_t)isr_14);
    set_idt_entry(0xF, (uint32_t)isr_15);
    set_idt_entry(0x10, (uint32_t)isr_16);
    set_idt_entry(0x11, (uint32_t)isr_17);
    set_idt_entry(0x12, (uint32_t)isr_18);

    // Use 0xF1 for syscalls; totally arbitrary
    set_idt_entry(0xF1, (uint32_t)isr_241);

    // IRQ ISRs
    set_idt_entry(0x20, (uint32_t)irq_0);
    set_idt_entry(0x21, (uint32_t)irq_1);
    set_idt_entry(0x28, (uint32_t)irq_15);

    interrupt_ptr.limit = sizeof(idt_descriptor)*256 - 1;
    interrupt_ptr.base = &interrupt_table;

    lidt(&interrupt_ptr);
}
