#include <log.h>

#include "amd64.h"
#include "gdt.h"

static struct gdt_descriptor gdt[5];
static struct gdtr gdtr;

void init_gdt()
{
	gdt[0].limit = 0;
	gdt[0].base_lo = 0;
	gdt[0].base_mi = 0;
	gdt[0].access = 0;
	gdt[0].flags = 0;
	gdt[0].base_hi = 0;

	gdt[1].limit = 0;
	gdt[1].base_lo = 0;
	gdt[1].base_mi = 0;
	gdt[1].access = 0b10011010;
	gdt[1].flags = 0b00100000;
	gdt[1].base_hi = 0;

	gdt[2].limit = 0;
	gdt[2].base_lo = 0;
	gdt[2].base_mi = 0;
	gdt[2].access = 0b10010010;
	gdt[2].flags = 0;
	gdt[2].base_hi = 0;

	gdt[3].limit = 0;
	gdt[3].base_lo = 0;
	gdt[3].base_mi = 0;
	gdt[3].access = 0b11111010;
	gdt[3].flags = 0b00100000;
	gdt[3].base_hi = 0;

	gdt[4].limit = 0;
	gdt[4].base_lo = 0;
	gdt[4].base_mi = 0;
	gdt[4].access = 0b11110010;
	gdt[4].flags = 0;
	gdt[4].base_hi = 0;

	gdtr.size = sizeof(gdt) - 1;
	gdtr.offset = (uint64_t)&gdt;

	gdt_reload((uint64_t)&gdtr);

	klog(LOG_DONE, "Loaded Global Descriptor Table\n");
}
