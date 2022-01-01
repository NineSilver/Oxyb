bits 64
section .text

global gdt_reload
gdt_reload:
	lgdt [rdi]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	pop rdi
	mov rax, 0x08
	push rax
	push rdi
	retfq

global idt_reload
idt_reload:
    lidt [rdi]
    ret

global read_cr2
read_cr2:
    mov rax, cr2
    ret

global write_cr3
write_cr3:
    mov cr3, rdi
    ret

global read_msr
read_msr:
    mov rcx, rdi
    rdmsr
    shl rdx, 32
    or rax, rdx
    ret

global outb
outb:
    mov rax, rsi
    mov rdx, rdi
    out dx, al
    ret

global outw
outw:
    mov rax, rsi
    mov rdx, rdi
    out dx, ax
    ret

global outd
outl:
    mov rax, rsi
    mov rdx, rdi
    out dx, eax
    ret

global inb
inb:
    mov rdx, rdi
    in al, dx
    ret

global inw
inw:
    mov rdx, rdi
    in ax, dx
    ret

global ind
inl:
    mov rdx, rdi
    in eax, dx
    ret

global __hlt
__hlt:
    hlt
    ret

global __cli
__cli:
    cli
    ret

global __sti
__sti:
    sti
    ret
