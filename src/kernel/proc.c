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
    proc_load_idle_binary(base_proc);
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
    proc_t *process = kmalloc(sizeof(proc_t));
    process->pid = pid;
    process->next = NULL;

    process->cs_base = process->eip = PROC_BIN_BASE;
    process->ss_base = process->esp = PROC_STACK_BASE;

    // Allocate physical memory for this process' code/data and stack
    // segments, but don't map it into memory yet.
    process->cs_p_addr = pmem_alloc();
    process->ss_p_addr = pmem_alloc();

    return process;
}

void proc_vmem_map(proc_t *proc)
{
    vmem_map(proc->cs_base, proc->cs_p_addr, 0x7);
    vmem_map(proc->ss_base, proc->ss_p_addr, 0x7);
}

void proc_vmem_unmap(proc_t *proc)
{
    // Not necessary to do anything here, as currently all processes
    // share the same virtual addresses.
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

    proc_vmem_map(executing_proc);
    crunchatize_me_capn(executing_proc->eip, executing_proc->esp);
}

void proc_load_binary(proc_t *proc, void *bin, uint32_t len)
{
    proc_vmem_map(proc);
    memcpy((void*)proc->cs_base, bin, len);
}

void proc_load_idle_binary(proc_t *proc)
{
    char* program = "\x90\x90\xeb\xfe";

    proc_vmem_map(proc);
    memcpy((void*)proc->cs_base, program, 4);
}

void drop_to_usermode()
{
    executing_proc = base_proc;
    proc_vmem_map(executing_proc);
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

    proc_vmem_unmap(switch_from);
    proc_vmem_map(executing_proc);

    crunchatize_me_capn(executing_proc->eip, executing_proc->esp);
}
