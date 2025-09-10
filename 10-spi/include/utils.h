/**********************************************************************************
 ** file         : utils.h
 ** description  : simple implementations of libc functions
 ** 
 **********************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define ARR_SIZE(x)     (sizeof(x) / sizeof((x)[0]))

void *memcpy(void *dest, const void *src, size_t len);
void *memset(void *dest, int c, size_t len);
size_t strlen(const char *str);

#endif
