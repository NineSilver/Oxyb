#include "amd64.h"

uint8_t inb(uint16_t port)
{
    uint8_t data;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(data)
        : "d"(port)
    );
    return data;
}

uint16_t inw(uint16_t port)
{
    uint16_t data;
    __asm__ volatile (
        "inw %1, %0"
        : "=a"(data)
        : "d"(port)
    );
    return data;
}

uint32_t inl(uint16_t port)
{
    uint32_t data;
    __asm__ volatile (
        "inl %1, %0"
        : "=a"(data)
        : "d"(port)
    );
    return data;
}

void outb(uint16_t port, uint8_t data)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(data), "d"(port)
    );
}

void outw(uint16_t port, uint16_t data)
{
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"(data), "d"(port)
    );
}

void outl(uint16_t port, uint32_t data)
{
    __asm__ volatile (
        "outl %0, %1"
        :
        : "a"(data), "d"(port)
    );
}
