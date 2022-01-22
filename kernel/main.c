#include <log.h>
#include <misc.h>

#include "arch/amd64.h"
#include "arch/gdt.h"
#include "arch/interrupts.h"
#include "boot/boot.h"
#include "mem/mm.h"
#include "sys/acpi.h"
#include "sys/apic.h"
#include "sys/pci.h"
#include "sys/pit.h"
#include "sys/serial.h"
#include "sys/smp.h"

void kmain(struct stivale2_struct* info)
{
    info = (void*)info + MEM_PHYS_OFFSET;

    init_serial(0x3f8);
    init_logging(&serial_putc);

    klog(LOG_NOPREFIX, "\nOxyb kernel | Hello, World!\n");
    
    init_gdt();
    init_idt();

    init_pmm(stivale2_get_tag(info, STIVALE2_STRUCT_TAG_MEMMAP_ID));
    init_vmm();

    init_acpi(stivale2_get_tag(info, STIVALE2_STRUCT_TAG_RSDP_ID));
    init_pit(1000);
    init_apic();
    init_pci();

    smp_boot_aps();

    klog(LOG_INFO, "End of kmain()\n");

    halt();
}
