#pragma once

#include "nerd.h"
#include "nerd_math.h"

#ifdef malloc
#undef malloc
#undef free
#undef realloc
#endif

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mem_allocation
{
    char *file;
    int line;
    size_t size;
    struct mem_allocation *prev, *next;
};

void *mem__malloc(size_t size, char *file, int line);
void mem__free(void *ptr);
void *mem__realloc(void *ptr, size_t size, char *file, int line);
void *mem__calloc(size_t num_items, size_t size, char *file, int line);
char *mem__strdup(char *str, char *file, int line);
void mem_dump();

#define malloc(size) mem__malloc(size, __FILE__, __LINE__)
#define realloc(ptr, size) mem__realloc(ptr, size, __FILE__, __LINE__)
#define calloc(num_items, size) mem__calloc(num_items, size, __FILE__, __LINE__)
#define free(ptr) mem__free(ptr)
#define strdup(ptr) mem__strdup(ptr, __FILE__, __LINE__)
