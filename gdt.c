#include "gdt.h"

struct gdtr gdtr_reg;
struct gdt_entry *gdt = (struct gdt_entry *)0x00000800;


void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;

    gdt[num].access      = access;
}

void gdt_init() {
    gdtr_reg.limit = (sizeof(struct gdt_entry) * 7) - 1;
    gdtr_reg.base  = 0x00000800;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF); // Kernel Code
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF); // Kernel Data
    gdt_set_gate(3, 0, 0xFFFFF, 0x92, 0xCF); // Kernel Stack
    gdt_set_gate(4, 0, 0xFFFFF, 0xFA, 0xCF); // User Code
    gdt_set_gate(5, 0, 0xFFFFF, 0xF2, 0xCF); // User Data
    gdt_set_gate(6, 0, 0xFFFFF, 0xF2, 0xCF); // User Stack

    load_gdt((uint32_t)&gdtr_reg);
}