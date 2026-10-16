#include "syscall.h"

void syscall_handler_c(struct cpu_registers *regs) {
    uint32_t syscall_no = regs->eax;

    switch (syscall_no) {
        case 0:
            terminal_putchar('\n');
            terminal_writestring("Syscall: Exit requested\n");
            break;

        case 1:
            terminal_putchar('\n');
            terminal_writestring((char *)regs->ebx);
            break;

        default:
            terminal_putchar('\n');
            terminal_writestring("Unknown System Call\n");
            break;
    }
}
