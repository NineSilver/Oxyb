#ifndef __OXYB__LIBK__STRING_H
#define __OXYB__LIBK__STRING_H

#include <stddef.h>
#include <stdint.h>

char* itoa(uintmax_t num, uint8_t base);

void* memset(void* dest, int c, size_t len);
void* memcpy(void* dest, const void* src, size_t len);

int strncmp(const char* s1, const char* s2, size_t n);

#endif /* !__OXYB__LIBK__STRING_H */
