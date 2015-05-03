#ifndef _PROC_H
#define _PROC_H
#include <stdint.h>


#define PROC_BIN_BASE   0x0C000000
#define PROC_STACK_BASE 0x0D000000

typedef struct struct_proc
{
    uint8_t pid;

    uint32_t cs_base;
    uint32_t ss_base;

    // TODO - Need to be able to store multiple code/data/stack physical
    // frames per process (linked list?)
    uint32_t cs_p_addr;
    uint32_t ss_p_addr;

    uint32_t esp;
    uint32_t eip;

    struct struct_proc *next;
} proc_t;


// Public interface
void init_proc();
proc_t* proc_spawn();
void proc_exit();
void drop_to_usermode();
void proc_switch_from(uint32_t eip, uint32_t esp);

// Public interface (temporary)
void load_helloworld_binary(proc_t *proc);
void load_idle_binary(proc_t *proc);

// Internal interface
void proc_vmem_map(proc_t *proc);
void proc_vmem_unmap(proc_t *proc);
proc_t* proc_create(uint8_t pid);

#endif
