#ifndef _PROC_H
#define _PROC_H
#include <stdint.h>


#define PROC_BIN_BASE   0x0A000000
#define PROC_STACK_BASE 0x0B000000

typedef struct struct_proc
{
    uint8_t pid;
    uint32_t bin_base;
    uint32_t stack_base;

    uint32_t esp;
    uint32_t eip;

    struct struct_proc *prev;
} proc_t;


void init_proc();
proc_t* spawn_proc();
void drop_to_usermode();

void yield_from(uint32_t eip, uint32_t esp);

void load_helloworld_binary(proc_t *proc);
void load_idle_binary(proc_t *proc);

proc_t* create_proc(uint8_t pid, proc_t *prev);

#endif
