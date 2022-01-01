#include <misc.h>

__attribute__((noreturn)) void halt()
{
    for(;;)
    {
        __cli();
        __hlt();
    }
    __builtin_unreachable();
}