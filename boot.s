section .multiboot
header_start:
    dd 0x1BADB002            ; Magic number
    dd 0x02                  ; Flags (bit 1 requests memory map)
    dd -(0x1BADB002 + 0x02)  ; Checksum
header_end:

section .text
global _start
global load_gdt
extern kernel_main

_start:
    cli                      ; Disable interrupts
    mov esp, stack_top     ; Setup stack
    push ebx                 ; Push pointer to the multiboot info structure
    push eax                 ; Push magic number
    call kernel_main         ; Jump to C code
    hlt                      ; Halt if it returns

section .bss
align 16
stack_bottom:
    resb 16384                ; 16KB for stack
stack_top:
