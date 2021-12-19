#include <log.h>

#include "arch/amd64.h"
#include "arch/gdt.h"
#include "arch/interrupts.h"
#include "arch/serial.h"
#include "boot/boot.h"

void kmain(struct stivale2_struct* info)
{
    (void)info;
    init_serial(0x3f8);
    init_logging(&serial_putc);

    klog(LOG_NOPREFIX, "\nOxyb kernel | Hello, World!\n");
    
    init_gdt();
    init_idt();

    klog(LOG_INFO, "End of kmain()\n");

    for(;;)
    {
        __cli();
    	__hlt();
	}
}
