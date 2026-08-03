#ifndef TERMINAL_H
#define TERMINAL_H
#include <stddef.h>
#include <stdint.h>

#define NUM_TERMINALS 4

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_update_cursor(size_t x, size_t y);
void terminal_scroll(void);
void terminal_switch(size_t term_idx);

#endif
