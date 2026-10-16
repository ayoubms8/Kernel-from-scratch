#ifndef SIGNAL_PANIC_H
#define SIGNAL_PANIC_H

#include "string.h"
#include "io.h"
#include "stdio.h"

#define MAX_SIGNALS 32

struct cpu_registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;                       // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss;           // Pushed automatically by CPU
};

typedef void (*signal_handler_t)(void *context);

void signal_init(void);
int signal_register(uint8_t signal_no, signal_handler_t handler);
int signal_schedule(uint8_t signal_no, void *context);
void signal_process(void);

void panic_clean_registers(void);
void panic_save_stack(struct cpu_registers *regs);
void kernel_panic(struct cpu_registers *regs, const char *message);

#endif