BITS 64

%define MH_MAGIC_64 0xfeedfacf
%define CPU_TYPE_X86 7
%define CPU_ARCH_ABI64 0x01000000
%define CPU_SUBTYPE_X86_ALL 3
%define MH_EXECUTE 2
%define MH_NOUNDEFS 1
; 32 bytes
header:
        DD MH_MAGIC_64
        DD (CPU_TYPE_X86 | CPU_ARCH_ABI64)
        DD CPU_SUBTYPE_X86_ALL
        DD MH_EXECUTE
        DD 3
        DD filesize - __load_command_0
        DD MH_NOUNDEFS
        DD 0

%define LC_SEGMENT_64 0x19
%define VM_PROT_READ 0x1
%define VM_PROT_WRITE 0x2
%define VM_PROT_EXECUTE 0x4
%define PROT VM_PROT_EXECUTE | VM_PROT_WRITE | VM_PROT_READ

__load_command_0:
        DD LC_SEGMENT_64
        DD __load_command_0_end - __load_command_0 ; TODO sizeof(load_command_64) + sizeof(section_64) * segment->nsect
.load_command_name:
        DB '__CODE',0x0         
        TIMES 16+.load_command_name-$ DB 0 ; 56 bytes
        DQ 0
        DQ filesize
        DQ 0                    ; TODO fileoffset, 72
        DQ filesize             ; TODO filesize
        DD PROT
        DD PROT
        DD 0                    ; TODO nsects
        DD 0
__load_command_0_end:
       
%define LC_UNIXTHREAD 0x5
%define x86_THREAD_STATE64 0x4
%define x86_EXCEPTION_STATE64_COUNT 42
__load_command_1:
        DD LC_UNIXTHREAD                           ; cmd
        DD __load_command_1_end - __load_command_1 ; cmdsize
        DD x86_THREAD_STATE64                      ; flavor
        DD x86_EXCEPTION_STATE64_COUNT             ; count
        DQ 0x0, 0x0, 0x0, 0x0  ; rax, rbx , rcx , rdx
        DQ 0x0, 0x0, 0x0, 0x0  ; rdi, rsi, rbp, rsp 
        DQ 0x0, 0x0            ; r8, r9
        DQ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 ; r10, r11, r12, r13, r14, r15
        DQ __code_start, 0x00, 0x00, 0x00, 0x00 ; rip, rflags, cs, fs, gs
__load_command_1_end:
__load_command_2:
        DD        LC_SEGMENT_64                                    ; cmd
        dd         __load_command_2_end - __load_command_2
        db         '__PAGEZERO',0x0,0,0,0,0,0 ; segment name (pad to 16 bytes)
        DQ        0x0                                                ; vmaddr
        DQ        0                                        ; vmsize
        DQ        0                                                ; fileoff
        DQ        0                                                ; filesize
        DD        0                                                 ; maxprot
        DD        0                                                ; initprot
        DD        0x0                                                ; nsects
        DD        0x0                                                ; flags
__load_command_2_end:
__code_start:
        mov rsi, msg
        mov rdx, len
        mov rdi, 1
        mov rax, 0x2000004
        syscall
        mov rax, 0x2000001
        mov rdi, 0
        syscall

        msg db 'Hello, world!', 0xa, 0
        len equ $-msg

__code_end:
        TIMES 4096 DB 0
__eof:
        filesize equ $-$$
