#include "terminal.h"
#include "vga.h"
#include "io.h"
#include "string.h"
#include "commands.h"
#include "stdio.h"

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
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_scroll(void) {
    size_t bytes_to_copy = VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(uint16_t);
    ft_memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, bytes_to_copy);
    
    uint16_t empty_entry = vga_entry(' ', terminal_color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        terminal_buffer[index] = empty_entry;
    }

    ft_memmove((void*)VGA_MEMORY, terminal_buffer, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
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

    ft_memmove((void*)VGA_MEMORY, terminal_buffer, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
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

    if (terminal_row == VGA_HEIGHT) {
        terminal_scroll();
        terminal_row--;
    }
    
    terminal_update_cursor(terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, ft_strlen(data));
}

size_t get_last_char(size_t row)
{
    if (row >= VGA_HEIGHT)
        return 0;
    for (int col = VGA_WIDTH - 1; col >= 0; col--) {
        char c = (char)(terminal_buffer[row * VGA_WIDTH + col] & 0xFF);
        if (c != ' ')
            return (size_t)col;
    }

    return 0;
}

void get_line() {
    int mylimit = get_last_char(terminal_row);
    unsigned char line[VGA_WIDTH + 1] = {0};
    int line_length = 0;
    
    int row_offset = terminal_row * VGA_WIDTH;
    for (int i = 0; i <= mylimit && i < VGA_WIDTH; i++) {
        line[i] = (unsigned char)(terminal_buffer[row_offset + i]);
        line_length++;
    }
    line[line_length] = '\0';
    terminal_column = 0;
    handle_command((char *)line);
}

void handle_left_arrow(void)
{
    if (terminal_row == 0 && terminal_column == 0)
        return;

    if (terminal_column == 0) {
        terminal_row--;
        terminal_column = get_last_char(terminal_row);
    } else {
        terminal_column--;
    }

    terminal_update_cursor(terminal_column, terminal_row);
}

void handle_right_arrow(void)
{
    size_t last = get_last_char(terminal_row);

    if (terminal_row == VGA_HEIGHT - 1 &&
        terminal_column >= last)
        return;

    if (terminal_column >= last) {
        if (terminal_row < VGA_HEIGHT - 1) {
            terminal_row++;
            terminal_column = 0;
        }
    } else {
        terminal_column++;
    }

    terminal_update_cursor(terminal_column, terminal_row);
}

void handle_up_arrow(void)
{
    if (terminal_row == 0)
        return;

    terminal_row--;

    size_t last = get_last_char(terminal_row);
    if (terminal_column > last)
        terminal_column = last;

    terminal_update_cursor(terminal_column, terminal_row);
}

void handle_down_arrow(void)
{
    if (terminal_row >= VGA_HEIGHT - 1)
        return;

    terminal_row++;

    size_t last = get_last_char(terminal_row);
    if (terminal_column > last)
        terminal_column = last;

    terminal_update_cursor(terminal_column, terminal_row);
}

void cycle_terminal_color(void) {
    static uint8_t color_id = 0;

    color_id = (color_id + 1) % 3;

    uint8_t new_color;

    switch (color_id) {
        case 0:
            new_color = vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            break;
        case 1:
            new_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            break;
        default:
            new_color = vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            break;
    }

    terminal_color = new_color;
    terminals[active_terminal].color = new_color;

    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        uint8_t ch = terminal_buffer[i] & 0xFF;
        terminal_buffer[i] = vga_entry(ch, new_color);
    }

    ft_memmove((void *)VGA_MEMORY, terminal_buffer,VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
}

void clear_screen(void){
    terminals[active_terminal].row = 0;
    terminals[active_terminal].column = 0;
    terminals[active_terminal].color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminals[active_terminal].buffer[index] = vga_entry(' ', terminals[active_terminal].color);
        }
    }

    terminal_row = terminals[active_terminal].row;
    terminal_column = terminals[active_terminal].column;
    ft_memmove((void*)VGA_MEMORY, terminal_buffer, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
    terminal_update_cursor(terminal_column, terminal_row);
}

void terminal_switch(size_t term_idx) {
    if (term_idx >= NUM_TERMINALS || term_idx == active_terminal) return;

    terminals[active_terminal].row = terminal_row;
    terminals[active_terminal].column = terminal_column;
    terminals[active_terminal].color = terminal_color;

    active_terminal = term_idx;
    terminal_row = terminals[active_terminal].row;
    terminal_column = terminals[active_terminal].column;
    terminal_color = terminals[active_terminal].color;
    terminal_buffer = terminals[active_terminal].buffer;

    ft_memmove((void*)VGA_MEMORY, terminal_buffer, VGA_WIDTH * VGA_HEIGHT * sizeof(uint16_t));
    terminal_update_cursor(terminal_column, terminal_row);
}
