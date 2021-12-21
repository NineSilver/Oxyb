#ifndef __OXYB__MEM__PMM_H
#define __OXYB__MEM__PMM_H

#include <stddef.h>

#include "../boot/stivale2.h"

#define PAGE_SIZE ((size_t)4096)
#define MEM_PHYS_OFFSET ((uint64_t)0xffff800000000000)

void init_pmm(struct stivale2_struct_tag_memmap* memmap);

void* pmm_malloc(size_t pages);
void* pmm_calloc(size_t pages);
void* pmm_realloc(void* addr, size_t new_pages, size_t old_pages);
void pmm_free(void* addr, size_t pages);

#endif /* !__OXYB__MEM__PMM_H */
