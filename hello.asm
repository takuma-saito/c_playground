BITS 64
global _main
        
section .text
_main:
        lea rdi, [rel msg]
        call print

;; 関数コールは下に配置する
print:
        mov rax, rdi
loop:
        inc rax
        mov rdx, [rax]
        and rdx, 255
        cmp rdx, 0
        jnz loop
        lea rsi, [rel msg]
        sub rax, rsi
        mov rdx, rax ; length of "Hello, world\n"
        mov rax, 0x2000004
        mov rdi, 1
        syscall
        mov rax, 0x2000001
        mov rdi, 0
        syscall

section .data
        msg db 'Hello, world!', 0xa, 0
        