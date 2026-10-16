#include "string.h"

size_t ft_strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

int ft_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
#include "stdio.h"
int ft_strncmp(const char *s1, const char *s2, size_t size) {
    int i = 0;
    while (s1[i] && (s1[i] == s2[i]) && i < size) {
        s1++;
        s2++;
        i++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void *ft_memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
    
    if (src == dest) {
        return dest;
    }
    
    if (pdest < psrc) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            pdest[i - 1] = psrc[i - 1];
        }
    }
    return dest;
}
