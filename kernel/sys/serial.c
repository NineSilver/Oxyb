#include "../arch/amd64.h"
#include "serial.h"

static uint16_t _port;

void init_serial(uint16_t port)
{
    outb(port + 1, 0x00);
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);
    outb(port + 3, 0x03);
    outb(port + 2, 0xC7);
    outb(port + 4, 0x0B);
    outb(port + 4, 0x1E);
    outb(port + 0, 0xAE);

    outb(port + 4, 0x0F);

    _port = port;
}

void serial_putc(char c)
{
    if(c == '\n')
        serial_putc('\r');

    while(!(inb(_port + 5) & 0x20));
    outb(_port, c);
}
