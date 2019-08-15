BITS 64
global _main
        
section .text
print:
        mov rax, rdi
loop:
        inc rax
        mov rdx, [rax]
        cmp rdx, 0
        jnz loop
        lea rsi, [rel msg]
        sub rax, rsi
        mov rdx, rax  ; length of "Hello, world\n"
        mov rax, 0x2000004
        mov rdi, 1
        syscall
        ret

_main:
        lea rdi, [rel msg]
        call print
        mov rax, 0x2000001
        mov rdi, 0
        syscall

section .data
        msg db 'Hello, world!', 0xa, 0
        