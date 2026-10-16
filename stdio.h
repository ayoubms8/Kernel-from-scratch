#ifndef STDIO_H
#define STDIO_H

typedef __builtin_va_list va_list;
#define va_start(v, l) __builtin_va_start(v, l)
#define va_arg(v, l)   __builtin_va_arg(v, l)
#define va_end(v)      __builtin_va_end(v)

int printf(const char* format, ...);
int printk(const char* format, ...);
int print_hex(unsigned int value);
void echo(const char *str);

#endif
