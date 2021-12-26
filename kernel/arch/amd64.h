#ifndef __OXYB__ARCH__AMD64_H
#define __OXYB__ARCH__AMD64_H

#include <stdint.h>

/* Descriptors */

void gdt_reload(uint64_t descriptor);
void idt_reload(uint64_t descriptor);


/* Port IO */

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t data);
void outw(uint16_t port, uint16_t data);
void outd(uint16_t port, uint32_t data);


/* Control registers */

void write_cr3(uint64_t value);

/* Misc */

void __hlt();
void __cli();
void __sti();

#endif /* !__OXYB__ARCH__AMD64_H */
