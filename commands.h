#ifndef COMMANDS_H
#define COMMANDS_H

#include "string.h"
#include "io.h"
#include "terminal.h"

void kernel_reboot(void);
void kernel_halt(void);
void kernel_dump_stack_raw(void);
void handle_command(char *cmd);
void kernel_shutdown(void);
void kernel_memory_map(void);

#endif