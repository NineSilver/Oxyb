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
