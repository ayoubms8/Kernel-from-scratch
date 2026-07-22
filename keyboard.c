#include "keyboard.h"
#include "io.h"
#include "terminal.h"

const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,  ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0, '-', 0,   0,   0,  '+', 0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

void keyboard_poll(void) {
    // Read the PS/2 status register
    uint8_t status = inb(0x64);
    
    // Check if the output buffer bit (bit 0) is set
    if (status & 0x01) {
        // Read the scancode from the PS/2 data port
        uint8_t scancode = inb(0x60);
        
        // The highest bit (bit 7) is set for key releases (break codes).
        // We only want to print on key presses (make codes), so we ensure bit 7 is 0.
        if (!(scancode & 0x80)) {
            char c = scancode_to_ascii[scancode];
            if (c != 0) {
                terminal_putchar(c);
            }
        }
    }
}
