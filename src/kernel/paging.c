#include "paging.h"
#include "terminal.h"

static page_directory_t *kernel_dir;

/**
 * Physical memory management
 */
// TODO - Better physical memory allocator
static uint32_t frame_stack[512];
static uint32_t frame_stack_ptr;

uint32_t pmem_alloc()
{
    if (frame_stack_ptr == 0)
        return 0;

    frame_stack_ptr --;
    return frame_stack[frame_stack_ptr];
}

void pmem_free(uint32_t addr)
{
    frame_stack[frame_stack_ptr] = addr;
    frame_stack_ptr ++;
}

void init_pmem()
{
    frame_stack_ptr = 0;

    // Push available frame addresses onto the stack. Start addresses
    // immediately after the identity-mapped lower 4MB.
    for (int i = 1; i < 512; i++)
    {
        pmem_free((uint32_t)(0x400000 + (i * 0x1000)));
    }
}

/**
 * Virtual Memory Management
 */
bool vmem_alloc(uint32_t v_addr, bool user)
{
    uint32_t abs_idx = (v_addr / 0x1000);
    uint32_t table_idx = abs_idx / 1024;
    uint32_t page_idx = abs_idx % 1024;

    // If the page at `v_addr` is already mapped into virtual memory, ensure
    // it isn't overwritten.
    page_table_t *dir_table = DIR_TABLE_PTR(kernel_dir->tables[table_idx]);
    page_table_t *table = (page_table_t*)(0xFFC00000 + (table_idx * 4096));
    if (dir_table != 0x0 && table->pages[page_idx] != 0x0)
        return false;

    uint32_t p_addr = pmem_alloc();
    uint32_t flags = user ? 0x7 : 0x3;
    vmem_map(v_addr, p_addr, flags);

    return true;
}

void vmem_free(uint32_t v_addr)
{
    uint32_t abs_idx = (v_addr / 0x1000);
    uint32_t table_idx = abs_idx / 1024;
    uint32_t page_idx = abs_idx % 1024;

    page_table_t *table = (page_table_t*)(0xFFC00000 + (table_idx * 4096));

    uint32_t page = table->pages[page_idx];
    uint32_t p_addr = PAGE_PHYSICAL_ADDR(page);

    // Return the corresponding physical frame to the pool
    pmem_free(p_addr);

    // Disable this page entry & invaldate it in the TLB
    table->pages[page_idx] = 0x0;
    asm volatile ("invlpg (%0)" : : "r" (v_addr) : "memory");
}

void vmem_map(uint32_t v_addr, uint32_t p_addr, uint32_t flags)
{
    uint32_t abs_idx = (v_addr / 0x1000);
    uint32_t table_idx = abs_idx / 1024;
    uint32_t page_idx = abs_idx % 1024;

    // Get the page table at `table_idx` within the directory, then map its
    // address to virtual address space. If it does not exist, create it.
    page_table_t *table = DIR_TABLE_PTR(kernel_dir->tables[table_idx]);
    if (table != 0x0)
    {
        table = (page_table_t*)(0xFFC00000 + (table_idx * 4096));
    }
    else
    {
        table = vmem_create_table(table_idx, flags);
    }
    
    table->pages[page_idx] = p_addr | 0x7;
    asm volatile ("invlpg (%0)" : : "r" (v_addr) : "memory");
}

void vmem_unmap(uint32_t v_addr)
{
    uint32_t abs_idx = (v_addr / 0x1000);
    uint32_t table_idx = abs_idx / 1024;
    uint32_t page_idx = abs_idx % 1024;
    
    page_table_t *table = (page_table_t*)(0xFFC00000 + (table_idx * 4096));

    table->pages[page_idx] = 0x0;
    asm volatile ("invlpg (%0)" : : "r" (v_addr) : "memory");
}

page_table_t* vmem_create_table(uint32_t table_idx, uint32_t flags)
{
    // Allocate physical memory for this table
    page_table_t *table = (page_table_t*)pmem_alloc();

    // Map the table into the directory
    kernel_dir->tables[table_idx] = (uint32_t)(table) | flags;

    // Get the virtual address of the table itself (not its entry in
    // the directory) so it can be manipulated.
    page_table_t *v_table = (page_table_t*)(0xFFC00000 + (table_idx * 4096));

    // Clear the table's memory and return the *virtual* address of
    // the table.
    for (int i = 0; i < 1024; i++)
    {
        v_table->pages[i] = 0x0;
    }
    return v_table;
}

/** 
 * Paging Setup
 */
void init_paging()
{
    init_pmem();

    init_kernel_dir();
    init_identity_paging();

    enable_paging();

    kernel_dir = (page_directory_t*)(0xFFFFF000);
}

void init_kernel_dir()
{
    kernel_dir = (page_directory_t*)pmem_alloc();
    memset(kernel_dir, 0x0, 1024);

    // x86 recursive page directory trick; Set the last entry of the
    // directory to the directory's physical address.
    // Since the directory itself is a table, directory entries will be
    // treated as table entries and therefore mapped into virtual
    // memory automagically.
    kernel_dir->tables[1023] = (uint32_t)kernel_dir | 0x3;
}

void init_identity_paging()
{
    page_table_t *ident_table = (page_table_t*)pmem_alloc();
    for (int i = 0; i < 1024; i++)
    {
        ident_table->pages[i] = (i * 0x1000) | 0x3;
    }
    kernel_dir->tables[0] = (uint32_t)(ident_table) | 0x3;
}

void enable_paging()
{
    // Set cr3 to the address of the page directory
    asm volatile ("mov %0, %%cr3" : : "r" (&kernel_dir->tables));

    // Enable paging by setitng the paging bit of cr0
    uint32_t cr0;
    asm volatile ("mov %%cr0, %0" : "=r" (cr0));
    cr0 = cr0 | 0x80000000;
    asm volatile ("mov %0, %%cr0" : : "r" (cr0));
}
