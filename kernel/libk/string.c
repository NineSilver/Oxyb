#include <string.h>

char* itoa(uintmax_t num, uint8_t base)
{
    static const char repr[17]= "0123456789ABCDEF";
    static char buffer[64];
    char* i; 

    i = &buffer[63];
    *i = '\0';

    do 
    { 
        *--i = repr[num % base]; 
        num /= base; 
    } while(num != 0); 

    return i; 
}

void* memset(void* dest, int c, size_t len)
{
    uint8_t* p = (uint8_t*)dest;

    size_t i;
    for(i = 0; i < len; i++)
        p[i] = (uint8_t)c;

    return p;
}

void* memcpy(void* dest, const void* src, size_t len)
{
    const uint8_t* src_ptr = (const uint8_t*)src;
    uint8_t* dest_ptr = (uint8_t*) dest;

    size_t i;
    for(i = 0; i < len; i++)
        dest_ptr[i] = src_ptr[i];
        
    return dest;
}
