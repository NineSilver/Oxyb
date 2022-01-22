#ifndef __OXYB__SYS__SMP_H
#define __OXYB__SYS__SMP_H

#define SMP_TRAMPOLINE_BASE (0x1000)
#define SMP_TRAMPOLINE_FLAG (0x500)
#define SMP_TRAMPOLINE_PML4 (0x510)
#define SMP_TRAMPOLINE_GDT (0x520)
#define SMP_TRAMPOLINE_IDT (0x530)
#define SMP_TRAMPOLINE_STACK (0x540)
#define SMP_TRAMPOLINE_JUMP (0x550)

void smp_boot_aps();

#endif /* !__OXYB__SYS__SMP_H */
