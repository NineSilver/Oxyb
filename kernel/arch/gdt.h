#ifndef __OXYB__AMD64__GDT_H
#define __OXYB__AMD64__GDT_H

#include <stdint.h>

struct gdt_descriptor
{
	uint16_t limit;
	uint16_t base_lo;
	uint8_t base_mi;
	uint8_t access;
	uint8_t flags;
	uint8_t base_hi;
} __attribute__((packed));

struct gdtr
{
	uint16_t size;
	uint64_t offset;
} __attribute__((packed));

void init_gdt();

#endif /* !__OXYB__AMD64__GDT_H */
