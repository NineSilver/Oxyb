#include <log.h>

#include "../arch/amd64.h"
#include "pit.h"

uint32_t pit_read_count()
{
    outb(0x43, 0);
    uint32_t counter = inb(0x40);
    counter |= inb(0x40) << 8;

    return counter;
}

void pit_sleep(uint16_t ms)
{
    outb(0x43, 0x30);
    outb(0x40, ms & 0xff);
    outb(0x40, (ms >> 8) & 0xff);

    while(pit_read_count() == 0);
}

void init_pit(int hz)
{
    int divisor = 1193180 / hz;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xff);
    outb(0x40, (divisor >> 8) & 0xFF);

    klog(LOG_DONE, "pit: initialized with a frequency of %d hz\n", hz);
}
