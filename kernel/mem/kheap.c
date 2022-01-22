#include <string.h>

#include "mm.h"

void* kalloc(size_t size)
{
    size_t pages = size / PAGE_SIZE;

    if(size % PAGE_SIZE) pages++;

    void* ptr = pmm_calloc(pages + 1);
    if(!ptr) return NULL;

    ptr += MEM_PHYS_OFFSET;
    struct chunk_header* this = ptr;
    ptr += PAGE_SIZE;

    this->pages = pages;
    this->size = size;

    return ptr;
}

void* krealloc(void* ptr, size_t size)
{
    if(!ptr) return kalloc(size);
    if(!ptr && !size) return NULL;
    if(!size)
    {
        kfree(ptr);
        return (void*)0;
    }

    struct chunk_header* this = (struct chunk_header*)((size_t)ptr - PAGE_SIZE);
    if((this->size + PAGE_SIZE - 1) / PAGE_SIZE
         == (size + PAGE_SIZE - 1) / PAGE_SIZE)
    {
        this->size = size;
        return ptr;
    }

    void* new = kalloc(size);
    if(!new) return NULL;
    
    if(this->size > size)
        memcpy(new, ptr, size);
    else
        memcpy(new, ptr, this->size);

    kfree(ptr);
    return new;
}

void kfree(void* ptr)
{
    struct chunk_header* this = (struct chunk_header*)((size_t)ptr - PAGE_SIZE);
    pmm_free((void*)((size_t)this - MEM_PHYS_OFFSET), this->pages + 1);
}
