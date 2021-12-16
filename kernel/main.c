#include <log.h>

#include "arch/amd64.h"
#include "arch/gdt.h"
#include "arch/serial.h"
#include "boot/boot.h"

void kmain(struct stivale2_struct* info)
{
    (void)info;

	init_gdt();
	init_serial(0x3f8);
	init_logging(&serial_putc);

	printf("Hello, %s\n", "World!");

	__hlt();
}
