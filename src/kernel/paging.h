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


uint32_t vmem_alloc();
page_table_t* vmem_create_table(uint32_t table_idx);
void vmem_free(uint32_t addr);

void init_pmem();
uint32_t pmem_alloc();
void pmem_free(uint32_t);

void init_paging();
void init_kernel_dir();
void init_identity_paging();
void enable_paging();

#endif
