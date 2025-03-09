#ifndef NIKOS_MEMORY_H_

#define NIKOS_MEMORY_H_

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif // NIKOS_MEMORY_H_