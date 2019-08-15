BITS 64
global _main
        
section .text
_main:
        call shellcode

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
        call exit

shellcode:
        push rbp
        mov rbp, rsp
        sub rsp, 0x30
        xor rax, rax            ; TODO
        mov [rbp - 0x8], rax
        lea rax, [rel arg1]
        mov rbx, [rax]
        cmp rbx, 0
        jne .skip
        xor rax, rax
.skip:
        mov [rbp - 0x10], rax
        lea rax, [rel code]
        mov [rbp - 0x18], rax
        xor rax, rax            ; TODO
        mov [rbp - 0x20], rax
        lea rdi, [rel code]
        lea rsi, [rbp - 0x18]
        lea rdx, [rbp - 0x20]
        mov rax, 0x200003b
        syscall
        call exit

exit:
        mov rax, 0x2000001
        mov rdi, 0
        syscall

section .data
        msg db 'Hello, world!', 0xa, 0
        code db '/bin/ls', 0
        arg1 db '-al', 0
