global load_gdt

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
