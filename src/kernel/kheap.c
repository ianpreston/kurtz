#include <stddef.h>
#include "kheap.h"
#include "paging.h"
#include "terminal.h"


#define KHEAP_BASE 0xA00000
#define CHUNK_INCR 8
#define CHUNK_SIZE(x) ((uint32_t)(x->next) - ((uint32_t)(x) + sizeof(x)))

static kheap_header_t *base;
static uint32_t heap_top;

void init_kheap()
{
    // Allocate the first page starting from KHEAP_BASE
    vmem_alloc(KHEAP_BASE, false);
    heap_top = KHEAP_BASE + PAGE_SIZE;

    // Create one big chunk of virtual memory (the size of one page
    // minus the size of the header) available for allocation
    base = (kheap_header_t*)KHEAP_BASE;
    base->next = NULL;
}

void* kmalloc(uint32_t size)
{
    kheap_header_t *cur = base;
    while (true)
    {
        if (cur->in_use)
        {
            if (cur->next == NULL)
            {
                // Reached the end of the list; need to allocate more
                // virtual memory for the heap
                cur->next = (kheap_header_t*)more_kheap();
            }
            cur = cur->next;
            continue;
        }

        uint32_t min_size = size + (CHUNK_INCR - (size % CHUNK_INCR));
        if (CHUNK_SIZE(cur) < min_size)
        {
            cur = cur->next;
            continue;
        }

        // If there is enough room, resize `cur` to `min_size` and make
        // a new chunk from the remaining space
        kheap_header_t *new_header = (kheap_header_t*)((uint32_t)(cur + 1) + min_size + 1);
        if ((uint32_t)new_header < heap_top && CHUNK_SIZE(cur) >= CHUNK_INCR)
        {
            new_header->in_use = false;
            new_header->next = cur->next;
            cur->next = new_header;
        }

        cur->in_use = true;
        return (void*)(cur + 1);
    }
}

uint32_t more_kheap()
{
    uint32_t base = heap_top;
    heap_top += PAGE_SIZE;
    vmem_alloc(base, false);
    return base;
}

void kfree(void *mallocd)
{
    // Find the header of `mallocd` (return value from kmalloc) and set
    // its free flag
    kheap_header_t *header = (kheap_header_t*)((uint32_t)mallocd - sizeof(kheap_header_t));
    header->in_use = false;

    // Merge any free chunks following `header` into one big chunk
    kheap_header_t *cur = header->next;
    while (cur != NULL && !cur->in_use)
    {
        header->next = cur->next;
        cur = cur->next;
    }

}
