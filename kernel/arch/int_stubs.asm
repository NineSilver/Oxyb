bits 64

section .text

%macro ISR_NAME 1
dq __isr_%1
%endmacro

%macro ISR_ERR 1
__isr_%1:
    push qword %1
    jmp __isr_common
%endmacro

%macro ISR_NOERR 1
__isr_%1:
    push qword 0
    push qword %1
    jmp __isr_common
%endmacro

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR 30
ISR_NOERR 31

%assign i 32
%rep 224
    ISR_NOERR i
%assign i i+1
%endrep

extern isr_handler
__isr_common:
    cld
    pushaq
    mov rdi, rsp
    call isr_handler
    mov rsp, rax
    popaq
    add rsp, 16
    iretq

section .data

global __isr_vector
__isr_vector:
%assign i 0
%rep 256
    ISR_NAME i
%assign i i+1
%endrep
