#include <log.h>

#include "../arch/amd64.h"
#include "../mem/mm.h"
#include "acpi.h"
#include "apic.h"
#include "pit.h"

madt_lapic_t** madt_lapics = NULL;
static size_t madt_lapic_num = 0;

madt_ioapic_t** madt_ioapics = NULL;
static size_t madt_ioapic_num = 0;

madt_iso_t** madt_isos = NULL;
static size_t madt_iso_num = 0;

madt_nmi_t** madt_nmis = NULL;
static size_t madt_nmi_num = 0;

static uint64_t lapic_get_base()
{
    return (read_msr(0x1b) & 0xfffff000) + MEM_PHYS_OFFSET;
}

static uint32_t lapic_read(size_t reg)
{
    return *((volatile uint32_t*)(lapic_get_base() + reg));
}

static void lapic_write(size_t reg, uint32_t data)
{
    *((volatile uint32_t*)(lapic_get_base() + reg)) = data;
}

void lapic_eoi()
{
    lapic_write(LAPIC_EOI, 0x00);
}

uint8_t lapic_get_id()
{
    return (uint8_t)(lapic_read(LAPIC_APIC_ID) >> 24);
}

void lapic_send_ipi(uint8_t id, uint8_t vec)
{
    lapic_write(LAPIC_ICR1, (uint32_t)(id << 24));
    lapic_write(LAPIC_ICR0, vec);
}

void lapic_send_init(uint8_t id)
{
    lapic_write(LAPIC_ICR1, (uint32_t)(id << 24));
    lapic_write(LAPIC_ICR0, (5 << 8));
}

void lapic_send_sipi(uint8_t id, uint64_t entry)
{
    lapic_write(LAPIC_ICR1, (uint32_t)(id << 24));
    lapic_write(LAPIC_ICR0, (6 << 8) | (entry / PAGE_SIZE));
}

size_t lapic_num()
{
    return madt_lapic_num;
}

madt_lapic_t** lapic_vec()
{
    return madt_lapics;
}

static uint32_t ioapic_read(uint64_t ioapic_base, size_t reg)
{
    *((volatile uint32_t*)(ioapic_base + IOAPIC_REG_IOREGSEL)) = reg;
    return *((volatile uint32_t*)(ioapic_base + IOAPIC_REG_IOWIN));
}

static void ioapic_write(uintptr_t ioapic_base, size_t reg, uint32_t data)
{
    *((volatile uint32_t*)(ioapic_base + IOAPIC_REG_IOREGSEL)) = reg;
    *((volatile uint32_t*)(ioapic_base + IOAPIC_REG_IOWIN)) = data;
}

uint32_t ioapic_get_gsi_count(size_t i)
{
    return ((ioapic_read(madt_ioapics[i]->addr + MEM_PHYS_OFFSET, IOAPIC_REG_VER) & 0xff0000) >> 16);
}

static madt_ioapic_t* get_ioapic_by_gsi(uint32_t gsi)
{
    size_t i;
    for(i = 0; i < madt_ioapic_num; i++)
    {
        if(madt_ioapics[i]->gsi <= gsi && madt_ioapics[i]->gsi + ioapic_get_gsi_count(i) > gsi)
            return madt_ioapics[i];
    }

    return NULL;
}

static void ioapic_set_redirect(uint8_t id, uint32_t gsi, uint8_t vec, uint16_t flags, int status)
{
    madt_ioapic_t* ioapic = get_ioapic_by_gsi(gsi);
    if(!ioapic)
    {
        panic("apic: no I/O APIC matching GSI %u found. Halting.\n", gsi);
    }
    
    uint64_t redirect = vec;

    if(flags & 2)
        redirect |= (1 << 13);

    if (flags & 8)
        redirect |= (1 << 15);

    if(!status)
        redirect |= (1 << 16);

    redirect |= (uint64_t)id << 56;
    uint32_t ioredtbl = (gsi - (ioapic->gsi)) * 2 + 16;

    ioapic_write(ioapic->addr + MEM_PHYS_OFFSET, ioredtbl + 0, (uint32_t)redirect);
    ioapic_write(ioapic->addr + MEM_PHYS_OFFSET, ioredtbl + 1, (uint32_t)(redirect >> 32));
}

void ioapic_redirect_irq(uint8_t id, uint8_t irq, int status)
{
    size_t i;
    for(i = 0; i < madt_iso_num; i++)
    {
        if(madt_isos[i]->irq_src == irq)
        {
            ioapic_set_redirect(id, madt_isos[i]->gsi, madt_isos[i]->irq_src + 0x20, madt_isos[i]->flags, status);
            return;
        }
    }
    ioapic_set_redirect(id, irq, irq + 0x20, 0, status);
}

void init_apic()
{
    /* Disable the PIC and enable the LAPIC */
    outb(0xa1, 0xff);
    outb(0x21, 0xff);
    lapic_write(LAPIC_SPURIOUS_IVR, lapic_read(LAPIC_SPURIOUS_IVR) | (1 << 8) | 0xff);
    klog(LOG_INFO, "apic: disabled the PIC, now using APIC Interrupt Controller\n");

    /* Now parse the MADT to find the APICs */
    madt_t* madt = get_madt();

    size_t i;
    for(i = 0; i < (madt->header.len - (sizeof(madt_t)));)
    {
        madt_header_t* header = (madt_header_t*)&madt->entries[i];
        switch(header->id)
        {
            case 0:
                /* Local APIC */
                klog(LOG_INFO, "apic: found Local APIC #%u\n", madt_lapic_num);
                madt_lapics = krealloc(madt_lapics, (madt_lapic_num + 1) * sizeof(madt_lapic_t*));
                madt_lapics[madt_lapic_num++] = (madt_lapic_t*)header;
                break;

            case 1:
                /* I/O APIC */
                klog(LOG_INFO, "apic: found I/O APIC #%u\n", madt_ioapic_num);
                madt_ioapics = krealloc(madt_ioapics, (madt_ioapic_num + 1) * sizeof(madt_ioapic_t*));
                madt_ioapics[madt_ioapic_num++] = (madt_ioapic_t*)header;
                break;

            case 2:
                /* Interrupt Source Override */
                klog(LOG_INFO, "apic: found ISO #%u\n", madt_iso_num);
                madt_isos = krealloc(madt_isos, (madt_iso_num + 1) * sizeof(madt_iso_t*));
                madt_isos[madt_iso_num++] = (madt_iso_t*)header;
                break;

            case 4:
                /* Non Maskable Interrupt */
                klog(LOG_INFO, "apic: found NMI #%u\n", madt_nmi_num);
                madt_nmis = krealloc(madt_nmis, (madt_nmi_num + 1) * sizeof(madt_nmi_t*));
                madt_nmis[madt_nmi_num++] = (madt_nmi_t*)header;
                break;

            default:
                break;
        }

        i += header->length;
    }

    /* Then redirect I/O APIC IRQs */
    for(i = 0; i < 16; i++)
        ioapic_redirect_irq(lapic_get_id(), i, 1);

    klog(LOG_INFO, "apic: set up IRQ redirection\n");

    /* Lastly, init LAPIC timer in periodic mode (IRQ 0) */
    lapic_write(LAPIC_TIMER_DIV, 0x03);
    lapic_write(LAPIC_TIMER_INIT_COUNT, 0xffffffff);

    pit_sleep(10);
    lapic_write(LAPIC_TIMER_REG, 0x10000);
    uint32_t ticks_in_10ms = 0xffffffff - lapic_read(LAPIC_TIMER_COUNTER);
    
    lapic_write(LAPIC_TIMER_REG, 32 | 0x20000);
    lapic_write(LAPIC_TIMER_DIV, 0x03);
    lapic_write(LAPIC_TIMER_INIT_COUNT, ticks_in_10ms);

    lapic_write(LAPIC_ERROR_STATUS, 0x00);
    lapic_write(LAPIC_ERROR_STATUS, 0x00);

    klog(LOG_INFO, "apic: now using LAPIC timer\n");
    klog(LOG_DONE, "APIC initialized succesfully\n");
}
