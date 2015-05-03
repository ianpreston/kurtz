#ifndef _PAGING_H
#define _PAGING_H
#include <stdbool.h>
#include "libkernel.h"

#define PAGE_SIZE 4096

#define DIR_TABLE_PTR(table_idx) ((page_table_t*)(table_idx & 0xFFFFF000))
#define PAGE_PHYSICAL_ADDR(x) ((uint32_t)(x & 0xFFFFF000))

typedef struct page_table
{
    uint32_t pages[1024];
} page_table_t __attribute__((aligned(4096)));

typedef struct page_directory
{
    uint32_t tables[1024];
} page_directory_t __attribute__((aligned(4096)));


// Public interface
void init_paging();

// Virtual memory - Public high-level and low-level interfaces
bool vmem_alloc(uint32_t addr, bool user);
void vmem_free(uint32_t addr);

void vmem_map(uint32_t v_addr, uint32_t p_addr, uint32_t flags);
void vmem_unmap(uint32_t v_addr);

// Physical memory - Public low-level interface
uint32_t pmem_alloc();
void pmem_free(uint32_t);

// Internal interface
page_table_t* vmem_create_table(uint32_t table_idx, uint32_t flags);

void init_pmem();

void init_kernel_dir();
void init_identity_paging();
void enable_paging();

#endif
