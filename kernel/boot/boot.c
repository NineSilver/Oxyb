#include <stddef.h>

#include "../mem/mm.h"
#include "boot.h"

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
