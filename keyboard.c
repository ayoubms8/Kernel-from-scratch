#include "keyboard.h"
#include "io.h"
#include "terminal.h"
#include "signal_panic.h"

const char scancode_to_qwerty[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,  ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0, '-', 0,   0,   0,  '+', 0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

const char scancode_to_azerty[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',
    0,  'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 0, '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,  ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0, '-', 0,   0,   0,  '+', 0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static int alt_pressed = 0;
static int shift_pressed = 0;
static int current_layout = 0;

void keyboard_handler_c(void) {
    // 1. Read the raw scancode from the keyboard hardware
    uint8_t scancode = inb(0x60);

    // 2. Schedule the signal (Signal 1) and pass the scancode as the context payload
    signal_schedule(1, (void *)(uint32_t)scancode);

    // 3. Send the End of Interrupt (EOI) command immediately
    outb(0x20, 0x20);
}

void my_keyboard_callback(void *context) {
    // 1. Unpack the context back into our 8-bit scancode
    uint8_t scancode = (uint8_t)((uint32_t)context);

    // 2. Your exact original logic goes here!
    if (!(scancode & 0x80)) {
        if (scancode == 0x3F){
            current_layout = !current_layout;
        }
        else if (scancode == 0x1C) {
            get_line();
        }
        else if (scancode == 0x4B) {
            handle_left_arrow();
        }
        else if (scancode == 0x4D) {
            handle_right_arrow();
        }
        else if (scancode == 0x48) {
            handle_up_arrow();
        }
        else if (scancode == 0x50) {
            handle_down_arrow();
        }
        else if (scancode == 0x3B){
            terminal_switch(0);
        }
        else if (scancode == 0x3C){
            terminal_switch(1);
        }
        else if (scancode == 0x3D){
            terminal_switch(2);
        }
        else if (scancode == 0x3E){
            cycle_terminal_color();
        }
        else {
            char c = 0;
            if (current_layout == 0) {
                c = scancode_to_qwerty[scancode];
            } else {
                c = scancode_to_azerty[scancode];
            }
            if (c != 0) {
                terminal_putchar(c);
            }
        }
    }
}