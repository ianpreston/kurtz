#include <stddef.h>
#include "proc.h"
#include "paging.h"
#include "kheap.h"
#include "terminal.h"


extern void crunchatize_me_capn(uint32_t eip, uint32_t esp);

static proc_t *last_proc = NULL;
static proc_t *executing_proc = NULL;

void init_proc()
{
    last_proc = create_proc(0, NULL);
    load_idle_binary(last_proc);
}

proc_t* spawn_proc()
{
    last_proc = create_proc(last_proc->pid + 1, last_proc);
    return last_proc;
}

proc_t* create_proc(uint8_t pid, proc_t *prev)
{
    // TODO - Don't assume processes will fit in 4kb
    // TODO - Memory protection would be nice...

    proc_t *process = (proc_t*)kmalloc(sizeof(proc_t));
    process->pid = pid;

    process->bin_base = process->eip = PROC_BIN_BASE + (process->pid * 0x1000);
    process->stack_base = process->esp = PROC_STACK_BASE + (process->pid * 0x100);
    process->prev = prev;

    vmem_alloc(process->bin_base, true);
    vmem_alloc(process->stack_base, true);

    return process;
}

void load_helloworld_binary(proc_t *proc)
{
    char* program = "\x90\x90\x83\xc4\x04\xba\xef\xbe\xad\xde\xcd\xf1\xeb\xfe";
    memcpy((void*)proc->bin_base, program, 16);
}

void load_idle_binary(proc_t *proc)
{
    char* program = "\x90\x90\xeb\xfe";
    memcpy((void*)proc->bin_base, program, 4);
}

void drop_to_usermode()
{
    executing_proc = last_proc;
    crunchatize_me_capn(executing_proc->eip, executing_proc->esp);
}

void yield_from(uint32_t eip, uint32_t esp)
{
    printf("Process pid=%x yields\n", executing_proc->pid);

    executing_proc->eip = eip;
    executing_proc->esp = esp;

    if (executing_proc->prev != NULL)
    {
        executing_proc = executing_proc->prev;
    }
    else
    {
        executing_proc = last_proc;
    }

    printf("Switching to process pid=%x\n", executing_proc->pid);

    crunchatize_me_capn(executing_proc->eip, executing_proc->esp);
}
