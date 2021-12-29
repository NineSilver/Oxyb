#include <log.h>

#include "amd64.h"
#include "interrupts.h"

static struct idt_descriptor idt[256];
static struct idtr idtr;
extern uint64_t __isr_vector[256];

static void (*_irq_handlers[16]) = {0};

static const char* exception_msgs[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

static void remap_pic()
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void irq_set_handler(uint8_t irq, void (*handler)())
{
    _irq_handlers[irq] = handler;
}

void irq_unset_handler(uint8_t irq)
{
    _irq_handlers[irq] = 0;
}

static void raise_exception(struct stack_frame* stackframe)
{
    klog(LOG_NOPREFIX, "\n========= " "\033[31m" "Exception triggered" "\033[97m" " =========\n\n");
    klog(LOG_NOPREFIX, "  Exception code %x (%s) | Error %x\n\n", stackframe->isr_num, exception_msgs[stackframe->isr_num], stackframe->error);
    klog(LOG_NOPREFIX, "  RAX=%x RBX=%x RCX=%x RDX=%x\n", stackframe->rax, stackframe->rbx, stackframe->rcx, stackframe->rdx);
    klog(LOG_NOPREFIX, "  RDI=%x RSI=%x RBP=%x RSP=%x\n", stackframe->rdi, stackframe->rsi, stackframe->rbp, stackframe->rsp);
    klog(LOG_NOPREFIX, "  R8=%x  R9=%x  R10=%x R11=%x\n", stackframe->r8,  stackframe->r9,  stackframe->r10, stackframe->r11);
    klog(LOG_NOPREFIX, "  R12=%x R13=%x R14=%x R15=%x\n\n", stackframe->r12, stackframe->r13, stackframe->r14, stackframe->r15);

    if(stackframe->isr_num == 0xE)
        klog(LOG_NOPREFIX, "  Faulty address: CR2=%x\n\n", read_cr2());
    
    klog(LOG_NOPREFIX, "  At RIP=%x\n\n", stackframe->rip);
    klog(LOG_NOPREFIX, "==== System halted - Kernel panic ====\n");
}

extern uint64_t isr_handler(uint64_t rsp)
{
    struct stack_frame* stackframe = (struct stack_frame*)rsp;
    if(stackframe->isr_num < 32)
    {
        raise_exception(stackframe);
        __cli();
        __hlt();
    }
    else if(stackframe->isr_num > 31 && stackframe->isr_num < 49)
    {
        void (*handler)() = _irq_handlers[stackframe->isr_num - 32];
        if(handler)
            handler();
    }

    if(stackframe->isr_num >= 40)
        outb(0xA0, 0x20);
    
    outb(0x20, 0x20);

    return rsp;
}

static void idt_set_descriptor(uint8_t vector, uint64_t handler, uint8_t flags)
{
    idt[vector].handler_lo = handler & 0xFFFF;
    idt[vector].selector = 0x08;
    idt[vector].ist = 0;
    idt[vector].attributes = flags;
    idt[vector].handler_mi = (handler >> 16) & 0xFFFF;
    idt[vector].handler_hi = (handler >> 32) & 0xFFFFFFFF;
    idt[vector].zero = 0;
}

void init_idt()
{
    int i;
    for(i = 0; i < 256; i++)
        idt_set_descriptor(i, __isr_vector[i], 0x8E);

    idtr.size = (256 * sizeof(struct idt_descriptor)) - 1;
    idtr.offset = (uint64_t)&idt;

    idt_reload((uint64_t)&idtr);
    remap_pic();

    __sti();

    klog(LOG_DONE, "Loaded Interrupt Descriptor Table\n");
}
