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
