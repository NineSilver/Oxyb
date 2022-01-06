#ifndef __OXYB__SYS__PCI_H
#define __OXYB__SYS__PCI_H

#include <stddef.h>
#include <stdint.h>

struct pci_device
{
    uint16_t segment;
    uint8_t bus;
    uint8_t function;
    uint8_t device;
    uint16_t device_id;
    uint16_t vendor_id;
    uint8_t revision_id;
    uint8_t subclass;
    uint8_t class;
    uint8_t prog_if;
};

struct pci_header
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
};

#define MMIO 0
#define PIO 1

struct pci_bar
{
    uint64_t base;
    size_t size;
    int pio;
};

uint8_t pci_read_byte(struct pci_device* device, uint16_t offset);
uint16_t pci_read_word(struct pci_device* device, uint16_t offset);
uint32_t pci_read_dword(struct pci_device* device, uint16_t offset);

void pci_write_byte(struct pci_device* device, uint16_t offset, uint8_t data);
void pci_write_word(struct pci_device* device, uint16_t offset, uint16_t data);
void pci_write_dword(struct pci_device* device, uint16_t offset, uint32_t data);

struct pci_device* pci_search_device(uint8_t class, uint8_t subclass, uint8_t prog_if, size_t index);
struct pci_bar pci_get_bar(struct pci_device* device, uint8_t idx);

void init_pci();

#endif /* !__OXYB__SYS__PCI_H */
