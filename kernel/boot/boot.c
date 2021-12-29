#include <stddef.h>

#include "../mem/mm.h"
#include "boot.h"

static uint8_t stack[4096];

static struct stivale2_header_tag_framebuffer framebuffer_request = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0
    },
    
    .framebuffer_width  = 0,
    .framebuffer_height = 0
};

__attribute__((section(".stivale2hdr"), used)) struct stivale2_header header2 = {
	.entry_point = 0,
    .stack = (uint64_t)stack + sizeof(stack),
    .flags = 0,
    .tags = (uint64_t)&framebuffer_request
};

void* stivale2_get_tag(struct stivale2_struct* info, uint64_t id)
{
    struct stivale2_tag* tag = (struct stivale2_tag*)(info->tags + MEM_PHYS_OFFSET);
    while(tag)
    {
        if(tag->identifier == id)
            return tag;
            
        tag = (struct stivale2_tag*)(tag->next + MEM_PHYS_OFFSET);
    }
    return NULL;
}
