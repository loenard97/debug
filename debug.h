#ifndef DEBUG_H
#define DEBUG_H

void *debug_malloc(size_t size, const char* file, int line);
void debug_free(void* ptr, const char* file, int line);
void debug_check_alloc();

#include "debug/alloc.c"

#undef malloc
#undef free
#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__)
#define check_alloc()

#endif