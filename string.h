#ifndef STRING_H
#define STRING_H

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint64_t unsigned long long
#define size_t unsigned int

size_t ft_strlen(const char* str);
int ft_strcmp(const char *s1, const char *s2);
void *ft_memmove(void *dest, const void *src, size_t n);
int ft_strncmp(const char *s1, const char *s2, size_t size);

#endif
