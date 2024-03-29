#ifndef __OXYB__SYS__APIC_H
#define __OXYB__SYS__APIC_H

#include <stddef.h>
#include <stdint.h>

#include "acpi.h"

#define LAPIC_APIC_ID 0x20
#define LAPIC_SPURIOUS_IVR 0xf0
#define LAPIC_EOI 0xb0
#define LAPIC_TPR 0x80
#define LAPIC_ERROR_STATUS 0x280
#define LAPIC_TIMER_REG 0x320
#define LAPIC_TIMER_INIT_COUNT 0x380
#define LAPIC_TIMER_COUNTER 0x390
#define LAPIC_TIMER_DIV 0x3e0

#define LAPIC_ICR0 0x300
#define LAPIC_ICR1 0x310

#define IOAPIC_REG_IOREGSEL 0x00
#define IOAPIC_REG_IOWIN 0x10
#define IOAPIC_REG_VER 0x01

void lapic_eoi();
uint8_t lapic_get_id();
void lapic_send_ipi(uint8_t id, uint8_t vec);
void init_apic();

size_t lapic_num();
madt_lapic_t** lapic_vec();

void lapic_send_init(uint8_t id);
void lapic_send_sipi(uint8_t id, uint64_t entry);

#endif /* !_OXYB__SYS__APIC_H */
