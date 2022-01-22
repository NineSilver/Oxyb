%define SMP_TRAMPOLINE_BASE 0x1000

%define SMP_TRAMPOLINE_FLAG 0x500
%define SMP_TRAMPOLINE_PML4 0x510
%define SMP_TRAMPOLINE_GDT 0x520
%define SMP_TRAMPOLINE_IDT 0x530
%define SMP_TRAMPOLINE_STACK 0x540
%define SMP_TRAMPOLINE_JUMP 0x550

section .data

global smp_trampoline_start
smp_trampoline_start:

incbin "sys/smp.blob.bin"

global smp_trampoline_end
smp_trampoline_end:


section .text

global smp_prepare_trampoline
smp_prepare_trampoline:
    mov byte [SMP_TRAMPOLINE_FLAG], 0
    mov qword [SMP_TRAMPOLINE_JUMP], rdi
    mov qword [SMP_TRAMPOLINE_PML4], rsi
    mov qword [SMP_TRAMPOLINE_STACK], rdx

    mov rsi, smp_trampoline_start
    mov rdi, SMP_TRAMPOLINE_BASE
    mov rcx, smp_trampoline_end - smp_trampoline_start
    rep movsb

    sgdt [SMP_TRAMPOLINE_GDT]
    sidt [SMP_TRAMPOLINE_IDT]

    ret

global smp_check_flag
smp_check_flag:
    xor rax, rax
    mov al, byte [SMP_TRAMPOLINE_FLAG]
    ret
