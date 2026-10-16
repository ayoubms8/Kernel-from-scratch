#include "terminal.h"
#include "keyboard.h"
#include "stdio.h"
#include "gdt.h"
#include "idt.h"
#include "signal_panic.h"
#include "multiboot.h"

struct multiboot_info *mbi = 0;

void kernel_main(uint32_t magic, uint32_t addr)
{
  if (magic == MULTIBOOT_BOOTLOADER_MAGIC) {
      mbi = (struct multiboot_info *)addr;
  }

  gdt_init();
  idt_init();
  signal_init();
	terminal_initialize();
  
	terminal_writestring("42\n");
  
  printf("Testing printf: Hello %s! Char: [%c] Number: %d, Hex: 0x%x , %d %d\n", "World" , 'a', -42, 3735928559, 3245, 25);
  printk("This is a printk message.\n");
  
  signal_register(1, my_keyboard_callback);

  while (1) {
    signal_process();
    __asm__ volatile ("hlt");
  }
}
