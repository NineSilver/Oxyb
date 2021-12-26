#include <log.h>

#include "../arch/amd64.h"
#include "mm.h"

static struct vmm_pagemap kernel_pagemap;

void init_vmm()
{
    kernel_pagemap.pml4 = (uint64_t*)pmm_calloc(1);

    size_t i;
    for(i = 0; i < 0x80000000; i += PAGE_SIZE)
        vmm_map_page(&kernel_pagemap, i, i + MEM_KERN_OFFSET, 0x03);

    for(i = 0; i < 0x200000000; i += PAGE_SIZE)
        vmm_map_page(&kernel_pagemap, i, i + MEM_PHYS_OFFSET, 0x03);

    vmm_load_pagemap(&kernel_pagemap);
    klog(LOG_DONE, "vmm: mapped kernel and higher half\n");
}

static uint64_t* get_next_level(uint64_t* table, size_t index)
{
    if(!(table[index] & 1))
    {
        table[index] = (uint64_t)pmm_calloc(1);
        table[index] |= 0x03;
    }
    return (uint64_t*)((table[index] & ~(0x1ff)) + MEM_PHYS_OFFSET);
}

void vmm_load_pagemap(struct vmm_pagemap* pagemap)
{
    write_cr3((uint64_t)pagemap->pml4);
}

struct vmm_pagemap vmm_new_pagemap()
{
    return (struct vmm_pagemap){
        .pml4 = pmm_calloc(1)
    };
}

void vmm_map_page(struct vmm_pagemap* pagemap, uint64_t physical, uint64_t virtual, uint64_t flags)
{
    size_t pml4_index = (size_t)(virtual & ((size_t) 0x1ff << 39)) >> 39;
    size_t pml3_index = (size_t)(virtual & ((size_t) 0x1ff << 30)) >> 30;
    size_t pml2_index = (size_t)(virtual & ((size_t) 0x1ff << 21)) >> 21;
    size_t pml1_index = (size_t)(virtual & ((size_t) 0x1ff << 12)) >> 12;

    uint64_t* pml3 = get_next_level(pagemap->pml4, pml4_index);
    uint64_t* pml2 = get_next_level(pml3, pml3_index);
    uint64_t* pml1 = get_next_level(pml2, pml2_index);

    pml1[pml1_index] = physical | flags;
}

void vmm_unmap_page(struct vmm_pagemap* pagemap, uint64_t virtual)
{
    size_t pml4_index = (size_t)(virtual & ((size_t) 0x1ff << 39)) >> 39;
    size_t pml3_index = (size_t)(virtual & ((size_t) 0x1ff << 30)) >> 30;
    size_t pml2_index = (size_t)(virtual & ((size_t) 0x1ff << 21)) >> 21;
    size_t pml1_index = (size_t)(virtual & ((size_t) 0x1ff << 12)) >> 12;

    uint64_t* pml3 = get_next_level(pagemap->pml4, pml4_index);
    uint64_t* pml2 = get_next_level(pml3, pml3_index);
    uint64_t* pml1 = get_next_level(pml2, pml2_index);

    pml1[pml1_index] = 0;
}
