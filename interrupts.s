global isr_dummy_stub
global load_idtr
global isr_keyboard_stub
global load_gdt
global isr_syscall_stub

global isr_vector_0
global isr_vector_1
global isr_vector_2
global isr_vector_3
global isr_vector_4
global isr_vector_5
global isr_vector_6
global isr_vector_7
global isr_vector_8
global isr_vector_9
global isr_vector_10
global isr_vector_11
global isr_vector_12
global isr_vector_13
global isr_vector_14
global isr_vector_15
global isr_vector_16
global isr_vector_17
global isr_vector_18
global isr_vector_19
global isr_vector_20
global isr_vector_21

extern keyboard_handler_c
extern exception_handler_c
extern syscall_handler_c

isr_dummy_stub:
    pusha                   ; Save registers
    ;add esp, 4             ; Clean up potential CPU error codes from the stack
    popa                   ; Restore registers
    iret                   ; Return safely

load_idtr:
    mov eax, [esp + 4]    ; Get the pointer argument passed from C
    lidt [eax]            ; Load the IDT pointer into the processor
    sti                   ; Enable hardware interrupts globally (Set Interrupt Flag)
    ret

isr_keyboard_stub:
    pusha                   ; Save all general-purpose registers (EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI)
    cld                     ; Clear Direction Flag (Required by x86 C Calling Convention!)
    call keyboard_handler_c
    popa                    ; Restore all general-purpose registers
    iret                    ; Return from interrupt (restores EIP, CS, EFLAGS)

load_gdt:
    mov eax, [esp + 4]
    lgdt [eax]
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ax, 0x18
    mov ss, ax
    
    jmp 0x08:flush_cs

flush_cs:
    ret

isr_syscall_stub:
    pusha                   ; Save general-purpose registers (EAX, EBX, ECX, EDX, etc.)
    mov eax, ds
    push eax                ; Save data segment

    push esp                ; Pass pointer to struct cpu_registers
    call syscall_handler_c  ; Jump to your C dispatcher
    add esp, 4              ; Clean up the stack argument

    pop eax
    mov ds, ax
    popa                    ; Restore registers
    iret                    ; Return to caller

; --- EXCEPTIONS WITHOUT ERROR CODES (Push dummy 0) ---
isr_vector_0:
    ;mov word [0xB8000], 0x4F30
    push 0
    push 0
    jmp exception_common_stub
isr_vector_1:
    push 0
    push 1
    jmp exception_common_stub
isr_vector_2:
    push 0
    push 2
    jmp exception_common_stub
isr_vector_3:
    push 0
    push 3
    jmp exception_common_stub
isr_vector_4:
    push 0
    push 4
    jmp exception_common_stub
isr_vector_5:
    push 0
    push 5
    jmp exception_common_stub
isr_vector_6:
    push 0
    push 6
    jmp exception_common_stub
isr_vector_7:
    push 0
    push 7
    jmp exception_common_stub
isr_vector_9:
    push 0
    push 9
    jmp exception_common_stub
isr_vector_15:
    push 0
    push 15
    jmp exception_common_stub
isr_vector_16:
    push 0
    push 16
    jmp exception_common_stub
isr_vector_18:
    push 0
    push 18
    jmp exception_common_stub
isr_vector_19:
    push 0
    push 19
    jmp exception_common_stub
isr_vector_20:
    push 0
    push 20
    jmp exception_common_stub

; --- EXCEPTIONS WITH ERROR CODES (CPU pushes error code, we only push vector #) ---
isr_vector_8:
    push 8
    jmp exception_common_stub
isr_vector_10:
    push 10
    jmp exception_common_stub
isr_vector_11:
    push 11
    jmp exception_common_stub
isr_vector_12:
    push 12
    jmp exception_common_stub
isr_vector_13:
    push 13
    jmp exception_common_stub
isr_vector_14:
    push 14
    jmp exception_common_stub
isr_vector_17:
    push 17
    jmp exception_common_stub
isr_vector_21:
    push 21
    jmp exception_common_stub

exception_common_stub:
    ;mov word [0xB8002], 0x4F43
    pusha           ; Push EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX
    mov eax, ds     ; Save data segment selector
    push eax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; Pass pointer to struct cpu_registers
    call exception_handler_c
    add esp, 4      ; Clean up stack pointer argument

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8      ; Clean up error code and interrupt number
    iret