#include <stddef.h>

#include "boot.h"

void* stivale2_get_tag(struct stivale2_struct* info, uint64_t id)
{
    struct stivale2_tag* tag = (void*)info->tags;
    for(;;)
    {
        if(!tag)
        {
            return NULL;
        }

        if(tag->identifier == id)
        {
            return tag;
        }

        tag = (void*)tag->next;
    }
}
