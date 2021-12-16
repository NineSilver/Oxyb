#ifndef __OXYB__AMD64__SERIAL_H
#define __OXYB__AMD64__SERIAL_H

#include <stdint.h>

void init_serial(uint16_t port);
void serial_putc(char c);

#endif /* !__OXYB__AMD64__SERIAL_H */
