#ifndef DEBUG_ALLOC_C
#define DEBUG_ALLOC_C

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <unistd.h>
#endif

#include <stddef.h>
#include <stdio.h>
#include "consts.c"

// #define DEBUG_INFO

typedef struct DebugMemoryBlock {
    size_t size;
    const char* file;
    int line;
    int free;
    struct DebugMemoryBlock *next;
} DebugMemoryBlock;

static DebugMemoryBlock *free_list = NULL;

void* debug_malloc(size_t size, const char* file, int line) {
    // printf("\033[1;32m[INFO]\033[0m %s:%d malloc of size %d\n", file, line, size);

    DebugMemoryBlock *current, *prev;
    void *result;

    if (size <= 0) {
        printf("%s malloc error size < 0\n", DEBUG_ERRO_STR);
        return NULL;
    }

    if (!free_list) {
        // printf("\033[1;32m[INFO]\033[0m malloc first allocation\n");
        #ifdef _WIN32
        current = VirtualAlloc(NULL, size + sizeof(DebugMemoryBlock), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        #endif
        #ifdef linux
        current = current = sbrk(size + sizeof(DebugMemoryBlock));
        #endif
        if (current == (void *) - 1) {
            printf("%s malloc error no allocation\n", DEBUG_ERRO_STR);
            return NULL;
        }

        current->size = size;
        current->file = file;
        current->line = line;
        current->free = 0;
        current->next = NULL;
        free_list = current;
    } else {
        // printf("\033[1;32m[INFO]\033[0m malloc later allocation\n");
        current = free_list;
        while (current) {
            if (current->free && current->size >= size) {
                current->free = 0;
                return (void *)(current + 1);
            }
            prev = current;
            current = current->next;
        }

        #ifdef _WIN32
        current = VirtualAlloc(NULL, size + sizeof(DebugMemoryBlock), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        #endif
        #ifdef linux
        current = current = sbrk(size + sizeof(DebugMemoryBlock));
        #endif
        if (!current) {
            printf("\033[1;31m[ERRO]\033[0m malloc error no allocation\n");
            return NULL;
        }

        current->size = size;
        current->file = file;
        current->line = line;
        current->free = 0;
        current->next = NULL;
        prev->next = current;
    }

    result = (void *)(current + 1);
    #ifdef DEBUG_INFO
    printf("%s %s:%d malloc 0x%p size %d\n", DEBUG_INFO_STR, file, line, result, size);
    #endif

    return result;
}

void debug_free(void* ptr, const char* file, int line) {
    if (!ptr) {
        return;
    }

    DebugMemoryBlock *block_ptr = (DebugMemoryBlock *)ptr - 1;

    if (block_ptr->free) {
        printf("%s %s:%d double free 0x%p size %d\n", DEBUG_ERRO_STR, file, line, ptr, block_ptr->size);
        return;
    }

    block_ptr->file = file;
    block_ptr->line = line;
    block_ptr->free = 1;

    #ifdef DEBUG_INFO
    printf("%s %s:%d free 0x%p size %d\n", DEBUG_INFO_STR, file, line, ptr, block_ptr->size);
    #endif
}

void debug_check_alloc() {
    DebugMemoryBlock *current, *prev;

    if (!free_list) {
        #ifdef DEBUG_INFO
        printf("%s no memory has been allocated\n", DEBUG_INFO_STR);
        #endif
        
        return;
    }

    current = free_list;
    while (current) {
        if (!current->free) {
            printf("%s %s:%d no free 0x%p size %d\n", DEBUG_WARN_STR, current->file, current->line, current + 1, current->size);
        }

        current = current->next;
    }
}

#endif