#include <log.h>

#include "../mem/mm.h"
#include "acpi.h"
#include "pci.h"

struct pci_device** pci_devices = NULL;
static size_t pci_device_num;

static uint64_t pci_get_device_addr(mcfg_entry_t* entry, uint8_t bus, uint8_t device, uint8_t function)
{
    return ((bus - entry->sbus) << 20 | device << 15 | function << 12) + MEM_PHYS_OFFSET;
}

static mcfg_entry_t* pci_get_mcfg_entry(uint16_t segment, uint8_t bus)
{
    mcfg_t* mcfg = get_mcfg();

    size_t i;
    for(i = 0; i < (mcfg->header.len - sizeof(mcfg_t)) / sizeof(mcfg_entry_t); i++)
    {
        if(mcfg->entries[i].seg_grp == segment && mcfg->entries[i].sbus <= bus && mcfg->entries[i].ebus >= bus)
            return &mcfg->entries[i];
    }

    panic("pci: unable to find a MCFG entry matching segment %u, bus %u\n", segment, bus);
}

uint8_t pci_read_byte(struct pci_device* device, uint16_t offset)
{
    return *(volatile uint8_t*)(pci_get_device_addr(pci_get_mcfg_entry(device->segment, device->bus), device->bus, device->device, device->function) + offset);
}

uint16_t pci_read_word(struct pci_device* device, uint16_t offset)
{
    return *(volatile uint16_t*)(pci_get_device_addr(pci_get_mcfg_entry(device->segment, device->bus), device->bus, device->device, device->function) + offset);
}

uint32_t pci_read_dword(struct pci_device* device, uint16_t offset)
{
    return *(volatile uint32_t*)(pci_get_device_addr(pci_get_mcfg_entry(device->segment, device->bus), device->bus, device->device, device->function) + offset);
}

void pci_write_byte(struct pci_device* device, uint16_t offset, uint8_t data)
{
    *(volatile uint8_t*)(pci_get_device_addr(pci_get_mcfg_entry(device->segment, device->bus), device->bus, device->device, device->function) + offset) = data;
}

void pci_write_word(struct pci_device* device, uint16_t offset, uint16_t data)
{
    *(volatile uint16_t*)(pci_get_device_addr(pci_get_mcfg_entry(device->segment, device->bus), device->bus, device->device, device->function) + offset) = data;
}

void pci_write_dword(struct pci_device* device, uint16_t offset, uint32_t data)
{
    *(volatile uint32_t*)(pci_get_device_addr(pci_get_mcfg_entry(device->segment, device->bus), device->bus, device->device, device->function) + offset) = data;
}

struct pci_device* pci_search_device(uint8_t class, uint8_t subclass, uint8_t prog_if, size_t index)
{
    size_t found = 0;
    
    size_t i;
    for(i = 0; i < pci_device_num; i++)
    {
        if(pci_devices[i]->class == class && pci_devices[i]->subclass == subclass && pci_devices[i]->prog_if == prog_if)
        {
            if(found != index)
                found++;
            else
                return pci_devices[i];
        }
    }

    return NULL;
}

struct pci_bar pci_get_bar(struct pci_device* device, uint8_t idx)
{
    if(idx > 5)
    {
        panic("pci: impossible to return a BAR greater than 5\n");
    }

    struct pci_bar ret;

    uint64_t bar = pci_read_dword(device, 0x10 + (idx * 4));
    int pio = bar & 1;
    ret.pio = pio;

    uint64_t bar_high = 0, bar_size_high = 0;

    if(pio)
    {
        ret.base = bar & 0xfffffffc;
    }
    else
    {
        uint8_t bitness = (bar >> 1) & 0x03;
        switch(bitness)
        {
            case 0:
                break;
            
            case 2:
                bar_high = pci_read_dword(device, 0x10 + (idx * 4) + 4);
                pci_write_dword(device, 0x10 + (idx * 4) + 4, 0xffffffff);
                bar_size_high = pci_read_dword(device, 0x10 + (idx * 4) + 4);
                pci_write_dword(device, 0x10 + (idx * 4) + 4, bar_high);
                break;
            
            default:
                panic("pci: BAR is not 32 now 64 bits (segment %u, bus %u, device %u, function %u)\n", device->segment, device->bus, device->device, device->function);
        };

        ret.base = ((bar_high << 32) | bar) & 0xfffffff0;
    }

    
    pci_write_dword(device, 0x10 + (idx * 4), 0xFFFFFFFF);
    uint64_t bar_size = pci_read_dword(device, 0x10 + (idx * 4));
    pci_write_dword(device, 0x10 + (idx * 4), bar);

    size_t size = ((bar_size_high << 32) | bar_size) & ~(pio ? (0xfffffffc) : (0xfffffff0));
    ret.size = ~size + 1;
    
    return ret;
}

static void pci_enumerate_function(uint64_t device_addr, size_t bus, uint8_t dev, uint8_t function)
{
    uint64_t function_addr = device_addr + (function << 12);
    struct pci_header* pci_function = (struct pci_header*)function_addr;
    
    if(!pci_function->device_id || pci_function->device_id == 0xffff)
        return;

    struct pci_device* device = kalloc(sizeof(struct pci_device));
    device->bus = bus;
    device->device = dev;
    device->function = function;
    device->device_id = pci_function->device_id;
    device->vendor_id = pci_function->vendor_id;
    device->revision_id = pci_function->revision_id;
    device->subclass = pci_function->subclass;
    device->class = pci_function->class;
    device->prog_if = pci_function->prog_if;

    pci_devices = krealloc(pci_devices, (pci_device_num + 1) * sizeof(struct pci_device));
    pci_devices[pci_device_num++] = device;

    klog(LOG_INFO, "pci: found device bus %u, slot %u, function %u | class %x, subclass %x\n", bus, dev, function, device->class, device->subclass);
}

static void pci_enumerate_dev(uint64_t bus_addr, size_t bus, uint8_t dev)
{
    uint64_t device_addr = bus_addr + (dev << 15);
    struct pci_header* pci_device = (struct pci_header*)device_addr;

    if(!pci_device->device_id || pci_device->device_id == 0xffff)
        return;

    uint8_t i;
    for(i = 0; i < 8; i++)
        pci_enumerate_function(device_addr, bus, dev, i);
}

static void pci_enumerate_bus(uint64_t base, size_t bus)
{
    uint64_t bus_addr = base + (bus << 20);
    struct pci_header* pci_bus = (struct pci_header*)bus_addr;
    
    if(!pci_bus->device_id || pci_bus->device_id == 0xffff)
        return;

    uint8_t i;
    for(i = 0; i < 32; i++)
        pci_enumerate_dev(bus_addr, bus, i);
}

void init_pci()
{
    mcfg_t* mcfg = get_mcfg();

    size_t i, j;
    for(i = 0; i < (mcfg->header.len - sizeof(mcfg_t)) / sizeof(mcfg_entry_t); i++)
    {    
        for(j = mcfg->entries[i].sbus; j < mcfg->entries[i].ebus; j++)
            pci_enumerate_bus(mcfg->entries[i].base_addr + MEM_PHYS_OFFSET, j);
    }

    klog(LOG_DONE, "PCI devices enumerated succesfully\n");
}
