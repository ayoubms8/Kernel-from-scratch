#include "commands.h"
#include "stdio.h"
#include "terminal.h"
#include "multiboot.h"

extern struct multiboot_info *mbi;

void print_hex_32(uint32_t val) {
    terminal_writestring("0x");
    for (int i = 28; i >= 0; i -= 4) {
        int rem = (val >> i) & 0xF;
        char c = rem < 10 ? '0' + rem : 'A' + (rem - 10);
        terminal_putchar(c);
    }
}

void kernel_memory_map(void) {
    if (!mbi || !(mbi->flags & (1 << 6))) {
        terminal_writestring("\nMemory map not provided by bootloader.\n");
        return;
    }

    struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)mbi->mmap_addr;
    uint32_t mmap_length = mbi->mmap_length;

    terminal_writestring("\n--- Memory Map ---\n");
    terminal_writestring("Base Address       | Length             | Type\n");
    terminal_writestring("-------------------|--------------------|------\n");

    for (uint32_t i = 0; i < mmap_length; i += mmap->size + sizeof(mmap->size)) {
        mmap = (struct multiboot_mmap_entry *)(mbi->mmap_addr + i);
        
        print_hex_32(mmap->addr_high);
        print_hex_32(mmap->addr_low);
        terminal_writestring(" | ");
        
        print_hex_32(mmap->len_high);
        print_hex_32(mmap->len_low);
        terminal_writestring(" | ");
        
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            terminal_writestring("Available\n");
        } else if (mmap->type == MULTIBOOT_MEMORY_RESERVED) {
            terminal_writestring("Reserved\n");
        } else if (mmap->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
            terminal_writestring("ACPI Reclaimable\n");
        } else if (mmap->type == MULTIBOOT_MEMORY_NVS) {
            terminal_writestring("NVS\n");
        } else if (mmap->type == MULTIBOOT_MEMORY_BADRAM) {
            terminal_writestring("Bad RAM\n");
        } else {
            terminal_writestring("Unknown\n");
        }
    }
}

void kernel_reboot(void) {
    uint8_t good = 0x02;
    while (good & 0x02) {
        // Read keyboard controller status (port 0x64)
        __asm__ volatile ("inb $0x64, %0" : "=a"(good));
    }
    outb(0x64, 0xFE);
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void kernel_shutdown(void) {
    __asm__ volatile ("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0x604)); // new qemu (port 0x604)    
    __asm__ volatile ("outw %0, %1" : : "a"((uint16_t)0x2000), "Nd"((uint16_t)0xB004)); // old  qemu (port 0xB004)
    __asm__ volatile ("outw %0, %1" : : "a"((uint16_t)0x3400), "Nd"((uint16_t)0x4004)); // virtualBox (port 0x3400)

    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}

void kernel_halt(void) {
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void kernel_dump_stack(void) {
    uint32_t *esp;
    __asm__ volatile ("mov %%esp, %0" : "=r"(esp));

    terminal_writestring("--- Raw Stack Dump (ESP upwards) ---\n");
    for (int i = 0; i < 20; i++) {
        terminal_writestring("0x");
        print_hex((uint32_t)(esp + i));
        terminal_writestring(": 0x");
        print_hex(esp[i]);
        terminal_writestring("\n");
    }
}

void PanikNow(void){
    
    __asm__ volatile("int $0");
}

void test_syscall(void) {
    const char *msg = "Hello from a simulated syscall!\n";
    __asm__ volatile (
        "mov $0, %%eax\n\t"
        "mov %0, %%ebx\n\t"
        "int $0x80\n\t"
        : : "r"(msg) : "eax", "ebx"
    );
}

void kernel_help(void) {
    terminal_writestring("\n--- Available Commands ---\n");
    terminal_writestring("help       - Show this help message\n");
    terminal_writestring("reboot     - Reboot the system\n");
    terminal_writestring("halt       - Halt the CPU\n");
    terminal_writestring("shutdown   - Power off the system (QEMU/VirtualBox)\n");
    terminal_writestring("clear      - Clear the screen\n");
    terminal_writestring("echo       - Print the given string\n");
    terminal_writestring("mystack    - Print the raw kernel stack\n");
    terminal_writestring("panik      - Trigger a kernel panic\n");
    terminal_writestring("test-write - Trigger a simulated syscall (sys_write)\n");
    terminal_writestring("mmap       - Display the physical memory map\n");
}

void handle_command(char *cmd) {
    if (ft_strcmp(cmd, "reboot") == 0) {
        kernel_reboot();
    }
    else if (ft_strcmp(cmd, "halt") == 0) {
        kernel_halt();
    }
    else if (ft_strncmp(cmd, "echo", 4) == 0) {
        echo(&cmd[5]);
    }
    else if (ft_strcmp(cmd, "mystack") == 0 || ft_strcmp(cmd, "print-kernel-stack-thing") == 0) {
        kernel_dump_stack();
    }
    else if (ft_strcmp(cmd, "shutdown") == 0){
        kernel_shutdown();
    }
    else if (ft_strcmp(cmd, "clear") == 0){
        clear_screen();
    }
    else if (ft_strcmp(cmd, "panik") == 0){
        PanikNow();
    }
    else if (ft_strcmp(cmd, "test-write") == 0){
        test_syscall();
    }
    else if (ft_strcmp(cmd, "mmap") == 0){
        kernel_memory_map();
    }
    else if (ft_strcmp(cmd, "help") == 0){
        kernel_help();
    }
     else {
        terminal_writestring("\nUnknown command.\n");
    }
}
