#ifndef IDT_H
#define IDT_H

#include "string.h"

// --- PIC Hardware I/O Ports ---
#define PIC1_COMMAND    0x20    // Master PIC Command Port
#define PIC1_DATA       0x21    // Master PIC Data Port
#define PIC2_COMMAND    0xA0    // Slave PIC Command Port
#define PIC2_DATA       0xA1    // Slave PIC Data Port
// --- Initialization Command Word 1 (ICW1) Flags ---
#define ICW1_ICW4       0x01    // Indicates that ICW4 will be sent
#define ICW1_INIT       0x10    // Initialization - required for both PICs
#define ICW1_CASCADE    (ICW1_INIT | ICW1_ICW4) // 0x11: Start cascade init
// --- Initialization Command Word 2 (ICW2): Vector Offsets ---
#define PIC1_OFFSET     0x20    // Master PIC vector offset: 32 (0x20..0x27)
#define PIC2_OFFSET     0x28    // Slave PIC vector offset:  40 (0x28..0x2F)
// --- Initialization Command Word 3 (ICW3): Cascade Wiring ---
#define PIC1_IRQ2_SLAVE 0x04    // Master: Tell it a Slave is wired to IRQ line 2 (0000 0100)
#define PIC2_CASCADE_ID 0x02    // Slave: Tell it its cascade identity number is 2
// --- Initialization Command Word 4 (ICW4): CPU Mode ---
#define ICW4_8086       0x01    // Set 8086/88 (MCS-80/85) microprocessor mode
// --- Interrupt Mask Register (IMR) Settings ---
#define PIC_UNMASK_KEYBOARD 0xFD // 1111 1101: Mute all Master IRQs EXCEPT IRQ 1 (Keyboard)
#define PIC_MUTE_ALL        0xFF // 1111 1111: Mute all IRQs on this chip

extern void load_idtr(uint32_t idtr_ptr);
extern void isr_keyboard_stub(void);
extern void isr_dummy_stub(void);
extern void isr_syscall_stub(void);

struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t  reserved;
    uint8_t  attributes;
    uint16_t isr_high;
} __attribute__((packed));

struct idtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern struct idt_entry idt[256] __attribute__((aligned(16)));
extern struct idtr idtr_reg;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init(void);
void pic_remap(void);
uint16_t retreave_cs(void);

#endif