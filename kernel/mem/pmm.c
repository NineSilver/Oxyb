#include <log.h>
#include <string.h>

#include "mm.h"

#define ALIGN_UP(n) (((n) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(n) ((n) & ~(PAGE_SIZE - 1))

#define BIT_SET(b, i) ((b)[(i) / 8] |= (1 << ((i) % 8)))
#define BIT_CLEAR(b, i) ((b)[(i) / 8] &= ~(1 << ((i) % 8)))
#define BIT_TEST(b, i) ((b)[(i) / 8] & (1 << ((i) % 8)))

static uint8_t* bitmap = 0;
static uint64_t highest_page = 0;

void init_pmm(struct stivale2_struct_tag_memmap* memmap)
{
    klog(LOG_INFO, "pmm: getting memory info\n");

    size_t i;
    for(i = 0; i < memmap->entries; i++)
    {
        if(memmap->memmap[i].type != STIVALE2_MMAP_USABLE
            && memmap->memmap[i].type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE
            && memmap->memmap[i].type != STIVALE2_MMAP_ACPI_RECLAIMABLE
            && memmap->memmap[i].type != STIVALE2_MMAP_KERNEL_AND_MODULES)
            continue;

        uint64_t top = memmap->memmap[i].base + memmap->memmap[i].length;

        if(top > highest_page)
            highest_page = top;
    }

    size_t bitmap_size = ALIGN_UP(ALIGN_DOWN(highest_page) / PAGE_SIZE / 8);

    for(i = 0; i < memmap->entries; i++)
    {
        if(memmap->memmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        if(memmap->memmap[i].length >= bitmap_size)
        {
            bitmap = (void*)(memmap->memmap[i].base + MEM_PHYS_OFFSET);
            memset(bitmap, 0xff, bitmap_size);

            memmap->memmap[i].length -= bitmap_size;
            memmap->memmap[i].base += bitmap_size;

            klog(LOG_INFO, "pmm: bitmap located at %x with size %d\n", bitmap, bitmap_size);

            break;
        }
    }

    for(i = 0; i < memmap->entries; i++)
    {
        if(memmap->memmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        size_t j;
        for(j = 0; j < memmap->memmap[i].length; j += PAGE_SIZE)
            BIT_CLEAR(bitmap, (memmap->memmap[i].base + j) / PAGE_SIZE);
    }

    klog(LOG_DONE, "pmm: bitmap initialized\n");
}

static void set_pages(void* addr, size_t pages)
{
    size_t i;
    for(i = 0; i < pages; i++)
        BIT_SET(bitmap, ((size_t)addr / PAGE_SIZE) + i);
}

static void clear_page(void* addr)
{
    BIT_CLEAR(bitmap, (size_t)addr / PAGE_SIZE);
}

void* pmm_malloc(size_t pages)
{
    size_t i, j;
    for(i = 0; i < highest_page / PAGE_SIZE; i++)
    {
        for(j = 0; j < pages; j++)
        {
            if(BIT_TEST(bitmap, i))
            {
                break;
            }
            else if(j == (pages - 1))
            {
                set_pages((void*)(i * PAGE_SIZE), pages);
                return (void*)(i * PAGE_SIZE);
            }
        }
    }

    return NULL;
}

void* pmm_calloc(size_t pages)
{
    void* new = pmm_malloc(pages);
    memset((void*)((size_t)new + MEM_PHYS_OFFSET), 0, pages * PAGE_SIZE);
    return new;
}

void* pmm_realloc(void* addr, size_t new_pages, size_t old_pages)
{
    void* new = pmm_malloc(new_pages);
    memcpy((void*)((size_t)new + MEM_PHYS_OFFSET), (void*)((size_t)addr + MEM_PHYS_OFFSET), old_pages / PAGE_SIZE);
    pmm_free(addr, old_pages);
    return new;
}

void pmm_free(void* addr, size_t pages)
{
    size_t i;
    for(i = 0; i < pages; i++)
        clear_page((void*)((size_t)addr + (i * PAGE_SIZE)));
}
