#ifndef __OXYB__MEM__PMM_H
#define __OXYB__MEM__PMM_H

#include <stddef.h>

#include "../boot/stivale2.h"

#define PAGE_SIZE ((size_t)4096)
#define MEM_PHYS_OFFSET ((uint64_t)0xffff800000000000)
#define MEM_KERN_OFFSET ((uint64_t)0xffffffff80000000)


/* PMM */

void init_pmm(struct stivale2_struct_tag_memmap* memmap);

void* pmm_malloc(size_t pages);
void* pmm_calloc(size_t pages);
void* pmm_realloc(void* addr, size_t new_pages, size_t old_pages);
void pmm_free(void* addr, size_t pages);


/* VMM */

struct vmm_pagemap
{
    uint64_t* pml4;
};

void init_vmm();
void vmm_load_pagemap(struct vmm_pagemap* pagemap);
struct vmm_pagemap vmm_new_pagemap();
void vmm_map_page(struct vmm_pagemap* pagemap, uint64_t physical, uint64_t virtual, uint64_t flags);
void vmm_unmap_page(struct vmm_pagemap* pagemap, uint64_t virtual);


/* Kernel heap */

struct chunk_header
{
    size_t pages;
    size_t size;
};

void* kalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

#endif /* !__OXYB__MEM__PMM_H */
