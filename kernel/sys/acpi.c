#include <stddef.h>

#include <log.h>
#include <string.h>

#include "../arch/amd64.h"
#include "../mem/mm.h"
#include "acpi.h"

static rsdp_t* rsdp;
static rsdt_t* rsdt;
static xsdt_t* xsdt;
static madt_t* madt;
static fadt_t* fadt;
static mcfg_t* mcfg;

static int xsdt_present;

void* acpi_search_table(char* signature, size_t index)
{
    size_t entries;

    if(!xsdt_present)
        entries = (rsdt->header.len - sizeof(rsdt->header)) / sizeof(uint32_t);
    else
        entries = (xsdt->header.len - sizeof(xsdt->header)) / sizeof(uint64_t);

    size_t i, count;
    for(i = 0, count = 0; i < entries; i++)
    {
        sdt_t* sdt;
        if(!xsdt_present)
          sdt = (sdt_t*)(rsdt->sptr[i] + MEM_PHYS_OFFSET);
        else
          sdt = (sdt_t*)(xsdt->sptr[i] + MEM_PHYS_OFFSET);

        if(!strncmp(signature, sdt->signature, 4))
        {
            if(count == index)
                return (void*)sdt;

            count++;
        }
    }

    return NULL;
}

madt_t* get_madt()
{
    return madt;
}

mcfg_t* get_mcfg()
{
    return mcfg;
}

void init_acpi(struct stivale2_struct_tag_rsdp* rsdp_tag)
{
    rsdp = (rsdp_t*)(rsdp_tag->rsdp + MEM_PHYS_OFFSET);
    xsdt_present = (rsdp->rev > 0 && rsdp->xsdt_addr);

    rsdt = (rsdt_t*)(rsdp->rsdt_addr + MEM_PHYS_OFFSET);
    if(xsdt_present)
        xsdt = (xsdt_t*)(rsdp->xsdt_addr + MEM_PHYS_OFFSET);

    if(!rsdt)
    {
        panic("ACPI: RSDT not found. Halting.\n");
    }

    fadt = acpi_search_table("FACP", 0);
    madt = acpi_search_table("APIC", 0);
    if(!madt)
    {
        panic("ACPI: MADT not found. Halting.\n");
    }
    
    mcfg = acpi_search_table("MCFG", 0);
    if(!mcfg)
    {
        panic("ACPI: MCFG not found. Halting.\n");
    }

    klog(LOG_INFO, "ACPI: RSDT at %x | FADT at %x | MADT at %x | MCFG at %x\n", (xsdt_present ? (uint64_t)xsdt : (uint64_t)rsdt), fadt, madt, mcfg);
    klog(LOG_DONE, "ACPI initialized\n");
}
