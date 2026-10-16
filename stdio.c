#include "stdio.h"
#include "terminal.h"
#include "string.h"

static int print_dec(int value) {
    int written = 0;
    if (value == 0) {
        terminal_putchar('0');
        return 1;
    }
    if (value < 0) {
        terminal_putchar('-');
        written++;
        value = -value;
    }
    
    char buffer[12];
    int i = 0;
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    
    while (i > 0) {
        terminal_putchar(buffer[--i]);
        written++;
    }
    return written;
}

int print_hex(unsigned int value) {
    int written = 0;
    if (value == 0) {
        terminal_putchar('0');
        return 1;
    }
    
    char buffer[10];
    int i = 0;
    while (value > 0) {
        int rem = value % 16;
        buffer[i++] = rem < 10 ? '0' + rem : 'a' + (rem - 10);
        value /= 16;
    }
    
    while (i > 0) {
        terminal_putchar(buffer[--i]);
        written++;
    }
    return written;
}

static int vprintf(const char* format, va_list args) {
    int written = 0;
    while (*format != '\0') {
        if (*format == '%') {
            format++;
            if (*format == 'd') {
                int value = va_arg(args, int);
                written += print_dec(value);
            } else if (*format == 'x') {
                unsigned int value = va_arg(args, unsigned long long);
                written += print_hex(value);
            } else if (*format == 's') {
                const char* s = va_arg(args, const char*);
                if (!s) s = "(null)";
                size_t len = ft_strlen(s);
                terminal_writestring(s);
                written += len;
            } else if (*format == 'c') {
                char c = (char)va_arg(args, int);
                terminal_putchar(c);
                written++;
            } else if (*format == '%') {
                terminal_putchar('%');
                written++;
            } else {
                terminal_putchar('%');
                terminal_putchar(*format);
                written += 2;
            }
        } else {
            terminal_putchar(*format);
            written++;
        }
        format++;
    }
    return written;
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}

int printk(const char* format, ...) {
    va_list args;
    va_start(args, format);
    terminal_writestring("[KERNEL] ");
    int ret = vprintf(format, args);
    va_end(args);
    return ret + 9;
}

void echo(const char *str) {
    if (str) {
        terminal_putchar('\n');
        terminal_writestring(str);
        terminal_putchar('\n');
    }
}
