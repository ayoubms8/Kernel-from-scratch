#include "terminal.h"
#include "vga.h"
#include "io.h"
#include "string.h"

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

typedef struct {
    size_t row;
    size_t column;
    uint8_t color;
    uint16_t buffer[VGA_WIDTH * VGA_HEIGHT];
} terminal_state_t;

terminal_state_t terminals[NUM_TERMINALS];
size_t active_terminal;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

void terminal_update_cursor(size_t x, size_t y) {
    uint16_t pos = y * VGA_WIDTH + x;
    
    // Send the low byte command to the VGA CRTC
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    
    // Send the high byte command to the VGA CRTC
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_scroll(void) {
    // Shift rows 1 through 24 upwards by one row
    size_t bytes_to_copy = VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(uint16_t);
    memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, bytes_to_copy);
    
    // Clear the last row (row 24) with blank spaces
    uint16_t empty_entry = vga_entry(' ', terminal_color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        terminal_buffer[index] = empty_entry;
    }

    // Update VGA memory
    memmove((void*)VGA_MEMORY, terminal_buffer, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
}

void terminal_initialize(void) 
{
    for (size_t i = 0; i < NUM_TERMINALS; i++) {
        terminals[i].row = 0;
        terminals[i].column = 0;
        terminals[i].color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        for (size_t y = 0; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                const size_t index = y * VGA_WIDTH + x;
                terminals[i].buffer[index] = vga_entry(' ', terminals[i].color);
            }
        }
    }

    active_terminal = 0;
    terminal_row = terminals[active_terminal].row;
    terminal_column = terminals[active_terminal].column;
    terminal_color = terminals[active_terminal].color;
    terminal_buffer = terminals[active_terminal].buffer;

    memmove((void*)VGA_MEMORY, terminal_buffer, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
    terminal_update_cursor(terminal_column, terminal_row);
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
    ((uint16_t*)VGA_MEMORY)[index] = terminal_buffer[index];
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
        }
    }

    // Trigger scrolling if we exceed the bottom row
    if (terminal_row == VGA_HEIGHT) {
        terminal_scroll();
        terminal_row--; // Keep the active row at the bottom
    }
    
    // Update the blinking hardware cursor to the new position
    terminal_update_cursor(terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void terminal_switch(size_t term_idx) {
    if (term_idx >= NUM_TERMINALS || term_idx == active_terminal) return;

    // Save current state
    terminals[active_terminal].row = terminal_row;
    terminals[active_terminal].column = terminal_column;
    terminals[active_terminal].color = terminal_color;

    // Switch
    active_terminal = term_idx;
    terminal_row = terminals[active_terminal].row;
    terminal_column = terminals[active_terminal].column;
    terminal_color = terminals[active_terminal].color;
    terminal_buffer = terminals[active_terminal].buffer;

    // Update screen
    memmove((void*)VGA_MEMORY, terminal_buffer, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
    terminal_update_cursor(terminal_column, terminal_row);
}
