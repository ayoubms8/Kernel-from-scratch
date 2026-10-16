#include "signal_panic.h"

static signal_handler_t signal_table[MAX_SIGNALS];

static uint8_t pending_signals[MAX_SIGNALS];

static void *pending_contexts[MAX_SIGNALS];

void signal_init(void) {
    for (int i = 0; i < MAX_SIGNALS; i++) {
        signal_table[i] = (signal_handler_t)0;
    }
}

int signal_register(uint8_t signal_no, signal_handler_t handler) {
    if (signal_no >= MAX_SIGNALS) return -1;
    signal_table[signal_no] = handler;
    return 0;
}

int signal_schedule(uint8_t signal_no, void *context) {
    if (signal_no >= MAX_SIGNALS) return -1;
    
    pending_contexts[signal_no] = context;
    pending_signals[signal_no] = 1;
    return 0;
}

void signal_process(void) {
    for (int i = 0; i < MAX_SIGNALS; i++) {
        if (pending_signals[i] == 1) {
            void *saved_context = pending_contexts[i];
            pending_signals[i] = 0;
            if (signal_table[i] != 0) {
                signal_table[i](saved_context); 
            }
        }
    }
}

void panic_clean_registers(void) {
    __asm__ volatile (
        "xor %%eax, %%eax\n\t"
        "xor %%ebx, %%ebx\n\t"
        "xor %%ecx, %%ecx\n\t"
        "xor %%edx, %%edx\n\t"
        "xor %%esi, %%esi\n\t"
        "xor %%edi, %%edi\n\t"
        ::: "eax", "ebx", "ecx", "edx", "esi", "edi"
    );
}

static struct cpu_registers saved_panic_regs;
void panic_save_stack(struct cpu_registers *regs) {
    if (regs) {
        saved_panic_regs = *regs;
    }
}

void kernel_panic(struct cpu_registers *regs, const char *message) {
    panic_save_stack(regs);

    panic_clean_registers();

    __asm__ volatile("cli");
    
    printf("\nKERNEL PANIC: %s\n", message);

    while (1) {
        __asm__ volatile("hlt");
    }
}

void exception_handler_c(struct cpu_registers *regs) {
    const char *panic_reason = "Unknown CPU Exception";
    // volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
    // vga[0] = 0x4F58; // 0x4F = Red Background/White Text, 0x58 = 'X'
    // vga[1] = 0x4F21; // '!'
    // vga[2] = 0x4F21; // '!'

    if (regs->int_no == 0x00) {
        panic_reason = "CPU Exception: Divide By Zero";
    }
    else if (regs->int_no == 0x01) {
        panic_reason = "CPU Exception: Debugger";
    }
    else if (regs->int_no == 0x02) {
        panic_reason = "CPU Exception: NMI";
    }
    else if (regs->int_no == 0x03) {
        panic_reason = "CPU Exception: Breakpoint";
    }
    else if (regs->int_no == 0x04) {
        panic_reason = "CPU Exception: Overflow";
    }
    else if (regs->int_no == 0x05) {
        panic_reason = "CPU Exception: Bounds";
    }
    else if (regs->int_no == 0x06) {
        panic_reason = "CPU Exception: Invalid Opcode";
    }
    else if (regs->int_no == 0x07) {
        panic_reason = "CPU Exception: Coprocessor not available";
    }
    else if (regs->int_no == 0x08) {
        panic_reason = "CPU Exception: Double fault";
    }
    else if (regs->int_no == 0x09) {
        panic_reason = "CPU Exception: Coprocessor Segment Overrun";
    }
    else if (regs->int_no == 0x0A) {
        panic_reason = "CPU Exception: Invalid Task State Segment";
    }
    else if (regs->int_no == 0x0B) {
        panic_reason = "CPU Exception: Segment not present";
    }
    else if (regs->int_no == 0x0C) {
        panic_reason = "CPU Exception: Stack Fault";
    }
    else if (regs->int_no == 0x0D) {
        panic_reason = "CPU Exception: General protection fault";
    }
    else if (regs->int_no == 0x0E) {
        panic_reason = "CPU Exception: Page fault";
    }
    else if (regs->int_no == 0x0F) {
        panic_reason = "CPU Exception: reserved";
    }
    else if (regs->int_no == 0x10) {
        panic_reason = "CPU Exception: Math Fault";
    }
    else if (regs->int_no == 0x11) {
        panic_reason = "CPU Exception: Alignment Check";
    }
    else if (regs->int_no == 0x12) {
        panic_reason = "CPU Exception: Machine Check";
    }
    else if (regs->int_no == 0x13) {
        panic_reason = "CPU Exception: SIMD Floating-Point Exception";
    }
    else if (regs->int_no == 0x14) {
        panic_reason = "CPU Exception: Virtualization Exception";
    }
    else if (regs->int_no == 0x15) {
        panic_reason = "CPU Exception: Control Protection Exception";
    }
    kernel_panic(regs, panic_reason);
}
