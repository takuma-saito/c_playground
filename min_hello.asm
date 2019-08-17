BITS 64
global _main
        
section .text
_main:
        mov rsi, msg
        mov rdx, len
        mov rdi, 1
        mov rax, 0x2000004
        syscall
        mov rax, 0x2000001
        mov rdi, 0
        syscall

section .data
        msg db 'Hello, world!', 0xa, 0
        len equ $-msg
