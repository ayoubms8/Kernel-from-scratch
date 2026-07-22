#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "terminal.h"
#include "keyboard.h"
#include "stdio.h"

#if defined(__linux__)
#error "standard compiler detected, use cross-compiler (i686-elf-gcc)"
#endif

#if !defined(__i386__)
#error "a ix86-elf compiler not found"
#endif

void kernel_main(void)
{
	/* Initialize terminal interface */
	terminal_initialize();

	/* Newline support is left as an exercise. */
	terminal_writestring("42\n");
    
    printf("Testing printf: Hello %s! Number: %d, Hex: 0x%x, Char: %c\n", "World", -42, 3735928559, 'A');
    printk("This is a printk message.\n");

  while (true) {
    keyboard_poll();
  }
}
