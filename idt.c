#include "idt.h"
#include "io.h"

struct idt_entry idt[256];
struct idtr idtr_reg;

void pic_remap(void) {
    uint8_t master_mask = inb(PIC1_DATA);
    uint8_t slave_mask  = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_CASCADE);
    outb(PIC2_COMMAND, ICW1_CASCADE);

    //remap IRQ 0-15 to vectors 32-47
    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);

    // connect master IRQ line 2 to the slave PIC chip
    outb(PIC1_DATA, PIC1_IRQ2_SLAVE);
    outb(PIC2_DATA, PIC2_CASCADE_ID);

    // configure chips to operate in 8086 x86 CPU mode
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // restore previously saved masks
    outb(PIC1_DATA, master_mask);
    outb(PIC2_DATA, slave_mask);
}

extern void load_idtr(uint32_t idtr_ptr);
extern void isr_keyboard_stub(void);
extern void isr_vector_0(void);
extern void isr_vector_1(void);
extern void isr_vector_2(void);
extern void isr_vector_3(void);
extern void isr_vector_4(void);
extern void isr_vector_5(void);
extern void isr_vector_6(void);
extern void isr_vector_7(void);
extern void isr_vector_8(void);
extern void isr_vector_9(void);
extern void isr_vector_10(void);
extern void isr_vector_11(void);
extern void isr_vector_12(void);
extern void isr_vector_13(void);
extern void isr_vector_14(void);
extern void isr_vector_15(void);
extern void isr_vector_16(void);
extern void isr_vector_17(void);
extern void isr_vector_18(void);
extern void isr_vector_19(void);
extern void isr_vector_20(void);
extern void isr_vector_21(void);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].isr_low    = (base & 0xFFFF);
    idt[num].isr_high   = (base >> 16) & 0xFFFF;
    idt[num].kernel_cs  = sel;
    idt[num].reserved   = 0;
    idt[num].attributes = flags; 
}

uint16_t retreave_cs(void){
    uint16_t cs = 0;
    __asm__ volatile("mov %%cs, %0":"=r"(cs));
    return cs;
}

void idt_init(void) {
    idtr_reg.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr_reg.base  = (uint32_t)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)isr_dummy_stub, retreave_cs(), 0x8E);
    }

    
    idt_set_gate(0,  (uint32_t)isr_vector_0,  retreave_cs(), 0x8E);
    idt_set_gate(1,  (uint32_t)isr_vector_1,  retreave_cs(), 0x8E);
    idt_set_gate(2,  (uint32_t)isr_vector_2,  retreave_cs(), 0x8E);
    idt_set_gate(3,  (uint32_t)isr_vector_3,  retreave_cs(), 0x8E);
    idt_set_gate(4,  (uint32_t)isr_vector_4,  retreave_cs(), 0x8E);
    idt_set_gate(5,  (uint32_t)isr_vector_5,  retreave_cs(), 0x8E);
    idt_set_gate(6,  (uint32_t)isr_vector_6,  retreave_cs(), 0x8E);
    idt_set_gate(7,  (uint32_t)isr_vector_7,  retreave_cs(), 0x8E);
    idt_set_gate(8,  (uint32_t)isr_vector_8,  retreave_cs(), 0x8E);
    idt_set_gate(9,  (uint32_t)isr_vector_9,  retreave_cs(), 0x8E);
    idt_set_gate(10, (uint32_t)isr_vector_10, retreave_cs(), 0x8E);
    idt_set_gate(11, (uint32_t)isr_vector_11, retreave_cs(), 0x8E);
    idt_set_gate(12, (uint32_t)isr_vector_12, retreave_cs(), 0x8E);
    idt_set_gate(13, (uint32_t)isr_vector_13, retreave_cs(), 0x8E);
    idt_set_gate(14, (uint32_t)isr_vector_14, retreave_cs(), 0x8E);
    idt_set_gate(15, (uint32_t)isr_vector_15, retreave_cs(), 0x8E);
    idt_set_gate(16, (uint32_t)isr_vector_16, retreave_cs(), 0x8E);
    idt_set_gate(17, (uint32_t)isr_vector_17, retreave_cs(), 0x8E);
    idt_set_gate(18, (uint32_t)isr_vector_18, retreave_cs(), 0x8E);
    idt_set_gate(19, (uint32_t)isr_vector_19, retreave_cs(), 0x8E);
    idt_set_gate(20, (uint32_t)isr_vector_20, retreave_cs(), 0x8E);
    idt_set_gate(21, (uint32_t)isr_vector_21, retreave_cs(), 0x8E);
    
    // remap hardware IRQs 0-15 to vectors 32-47
    pic_remap();

    // overwrite vector 33 (32 + IRQ 1) with the real keyboard handler so we can catch our keyboard input
    idt_set_gate(PIC1_OFFSET + 1, (uint32_t)isr_keyboard_stub, retreave_cs(), 0x8E);

    idt_set_gate(0x80, (uint32_t)isr_syscall_stub, 0x08, 0xEE);
    
    // mute all hardware interrupts on the PIC except for the keyboard
    outb(PIC1_DATA, PIC_UNMASK_KEYBOARD); 
    outb(PIC2_DATA, PIC_MUTE_ALL); 

    load_idtr((uint32_t)&idtr_reg);
}
