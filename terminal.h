#ifndef TERMINAL_H
#define TERMINAL_H
#include "string.h"

#define NUM_TERMINALS 3
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_update_cursor(size_t x, size_t y);
void terminal_scroll(void);
void terminal_switch(size_t term_idx);

void handle_left_arrow(void);
void handle_right_arrow(void);
void handle_up_arrow(void);
void handle_down_arrow(void);
void cycle_terminal_color(void);
void get_line(void);
void clear_screen(void);


#endif
