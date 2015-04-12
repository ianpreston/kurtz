#include <stddef.h>
#include "proc.h"
#include "paging.h"
#include "kheap.h"
#include "terminal.h"

extern void crunchatize_me_capn(uint32_t eip, uint32_t esp);

static proc_t *base_proc = NULL;
static proc_t *executing_proc = NULL;

void init_proc()
{
    base_proc = proc_create(1);
    load_idle_binary(base_proc);
}

proc_t* proc_spawn()
{
    proc_t *prev = base_proc;
    while (prev->next != NULL)
    {
        prev = prev->next;
    }

    prev->next = proc_create(prev->pid + 1);
    return prev->next;
}

proc_t* proc_create(uint8_t pid)
{
    // TODO - Don't assume processes will fit in 4kb
    // TODO - Memory protection would be nice...

    proc_t *process = (proc_t*)kmalloc(sizeof(proc_t));
    process->pid = pid;
    process->next = NULL;

    process->bin_base = process->eip = PROC_BIN_BASE + (process->pid * 0x1000);
    process->stack_base = process->esp = PROC_STACK_BASE + (process->pid * 0x100);

    vmem_alloc(process->bin_base, true);
    vmem_alloc(process->stack_base, true);

    return process;
}

void proc_exit()
{
    proc_t *exited = executing_proc;

    // Get the process immediately before `exited` in the list
    proc_t *cur = base_proc;
    while (cur->next != exited)
    {
        cur = cur->next;
    }

    // Remove exited from the linked list
    cur->next = exited->next;

    executing_proc = cur;
    printf("Process %x exited. Switching to %x\n", exited->pid, executing_proc->pid);

    kfree(exited);
    crunchatize_me_capn(executing_proc->eip, executing_proc->esp);
}

void load_helloworld_binary(proc_t *proc)
{
    char* program = "\xb8\x04\x00\x00\x00\xbb\x1a\x30\x00\x0a\xcd\xf1\xb8\x01\x00\x00\x00\xbb\x01\x00\x00\x00\xcd\xf1\xeb\xfe\x48\x65\x6c\x6c\x6f\x2c\x20\x77\x6f\x72\x6c\x64\x21\x0a\x00";
    memcpy((void*)proc->bin_base, program, 41);
}

void load_idle_binary(proc_t *proc)
{
    char* program = "\x90\x90\xeb\xfe";
    memcpy((void*)proc->bin_base, program, 4);
}

void drop_to_usermode()
{
    executing_proc = base_proc;
    crunchatize_me_capn(executing_proc->eip, executing_proc->esp);
}

void proc_switch_from(uint32_t eip, uint32_t esp)
{
    proc_t *switch_from = executing_proc;

    if (executing_proc->next != NULL)
    {
        executing_proc = executing_proc->next;
    }
    else
    {
        executing_proc = base_proc;
    }

    // debug: Log task switches as long as they are not from the idle
    // process to itself.
    if (switch_from->pid != 1 || executing_proc->pid != 1)
    {
        printf("Switching from process %x to %x\n", switch_from->pid, executing_proc->pid);
    }

    switch_from->eip = eip;
    switch_from->esp = esp;

    crunchatize_me_capn(executing_proc->eip, executing_proc->esp);
}
