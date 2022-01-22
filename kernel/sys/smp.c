#include <stdint.h>
#include <string.h>

#include <log.h>
#include <misc.h>

#include "../mem/mm.h"
#include "apic.h"
#include "pit.h"
#include "smp.h"

extern uint32_t smp_trampoline_start, smp_trampoline_end;

extern void smp_prepare_trampoline(void* jump_address, void* kernel_pml4, void* stack);
extern int smp_check_flag();

static void ap_kentry()
{
    klog(LOG_NOPREFIX, "A\n");
    halt();
}

void smp_boot_aps()
{
    klog(LOG_INFO, "smp: booting APs\n");
    uint8_t bsp_id = lapic_get_id();
    klog(LOG_INFO, "smp: BSP LAPIC ID is %u\n", bsp_id);

    size_t i;
    for(i = 0; i < lapic_num(); i++)
    {
        if(lapic_vec()[i]->apic_id == bsp_id)
            continue;

        uint8_t* stack = kalloc(8192);
        smp_prepare_trampoline(&ap_kentry, vmm_kernel_pagemap()->pml4, (void*)((uint64_t)&stack + 8192));

        lapic_send_init(lapic_vec()[i]->apic_id);
        pit_sleep(10);
        lapic_send_sipi(lapic_vec()[i]->apic_id, (uint32_t)SMP_TRAMPOLINE_BASE);

        while(!smp_check_flag());

        klog(LOG_INFO, "smp: brought up CPU #%u, LAPIC #%u\n", lapic_vec()[i]->proc_id, lapic_vec()[i]->apic_id);
    }

    klog(LOG_DONE, "smp: APs booted\n");
}
