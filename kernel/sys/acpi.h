#ifndef __OXYB__SYS__ACPI_H
#define __OXYB__SYS__ACPI_H

#include <stddef.h>
#include <stdint.h>

#include "../boot/stivale2.h"

typedef struct rsdp
{
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t rev;
    uint32_t rsdt_addr;

    uint32_t len;
    uint64_t xsdt_addr;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) rsdp_t;

typedef struct sdt
{
    char signature[4];
    uint32_t len;
    uint8_t rev;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_rev;
    uint32_t creator_id;
    uint32_t creator_rev;
} __attribute__((packed)) sdt_t;

typedef struct rsdt
{
    sdt_t header;
    uint32_t sptr[];
} __attribute__((packed)) rsdt_t;

typedef struct xsdt
{
    sdt_t header;
    uint64_t sptr[];
} __attribute__((packed)) xsdt_t;


typedef struct madt
{
    sdt_t header;
    uint32_t local_controller_address;
    uint32_t flags;
    uint8_t entries[0];
} __attribute__((packed)) madt_t;

typedef struct madt_header
{
    uint8_t id;
    uint8_t length;
} __attribute__((packed)) madt_header_t;

typedef struct madt_lapic
{
    madt_header_t header;
    uint8_t proc_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed)) madt_lapic_t;

typedef struct madt_ioapic
{
    madt_header_t header;
    uint8_t apic_id;
    uint8_t reserved;
    uint32_t addr;
    uint32_t gsi;
} __attribute__((packed)) madt_ioapic_t;

typedef struct madt_iso
{
    madt_header_t header;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((packed)) madt_iso_t;

typedef struct madt_nmi
{
    madt_header_t header;
    uint8_t processor;
    uint16_t flags;
    uint8_t lint;
} __attribute__((packed)) madt_nmi_t;

typedef struct mcfg_entry
{
    uint64_t base_addr;
    uint16_t seg_grp;
    uint8_t sbus;
    uint8_t ebus;
    uint32_t reserved;
} __attribute__((packed)) mcfg_entry_t;

typedef struct mcfg
{
    sdt_t header;
    uint64_t reserved;
    mcfg_entry_t entries[];
} __attribute__((packed)) mcfg_t;

typedef struct generic_address
{
    uint8_t addr_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t address;
} generic_address_t;

typedef struct fadt
{
    sdt_t header;
    uint32_t firmware_ctrl;
    uint32_t dsdt;

    uint8_t reserved1;

    uint8_t preferred_power_management_profile;
    uint16_t sci_int;
    uint32_t smi_cmd_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_control;
    uint32_t pm1_aevent_block;
    uint32_t pm1_bevent_block;
    uint32_t pm1_acontrol_block;
    uint32_t pm1_bcontrol_block;
    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;
    uint8_t pm_timer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t cstate_control;
    uint16_t worstc2_latency;
    uint16_t worstc3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;

    uint16_t boot_arch_flags;

    uint8_t reserved2;
    uint32_t flags;

    generic_address_t reset_reg;

    uint8_t reset_value;
    uint8_t reserved3[3];

    uint64_t x_firmware_control;
    uint64_t x_dsdt;

    generic_address_t x_pm1_aeventblock;
    generic_address_t x_pm1_beventblock;
    generic_address_t x_pm1_acontrolblock;
    generic_address_t x_pm1_bcontrolblock;
    generic_address_t x_pm2_controlblock;
    generic_address_t x_pm_timerblock;
    generic_address_t x_gpe0_block;
    generic_address_t x_gpe1_block;
} fadt_t;

void init_acpi(struct stivale2_struct_tag_rsdp* rsdp_tag);
void* acpi_search_table(char* signature, size_t index);
madt_t* get_madt();
mcfg_t* get_mcfg();

#endif /* !__OXYB__SYS__ACPI_H */
