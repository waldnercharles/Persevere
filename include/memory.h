#pragma once

#include "vec.h"

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
    s32 line;
    u32 size;
    struct mem_allocation *prev, *next;
};

static struct mem_allocation *allocation_head;

void *
mem__malloc(u32 size, char *file, s32 line)
{
    if (size == 0)
    {
        return NULL;
    }

    struct mem_allocation *alloc = malloc(size + sizeof(struct mem_allocation));
    if (alloc == NULL)
    {
        return NULL;
    }

    alloc->file = file;
    alloc->line = line;
    alloc->next = allocation_head;
    if (allocation_head != NULL)
    {
        alloc->next->prev = alloc;
    }

    alloc->prev = NULL;
    alloc->size = (s32)size;

    allocation_head = alloc;

    return alloc + 1;
}

void
mem__free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    struct mem_allocation *alloc = (struct mem_allocation *)ptr - 1;

    if (alloc->next != NULL)
    {
        alloc->next->prev = alloc->prev;
    }

    if (alloc->prev != NULL)
    {
        alloc->prev->next = alloc->next;
    }
    else
    {
        allocation_head = alloc->next;
    }

    free(ptr);
}

void *
mem__realloc(void *ptr, u32 size, char *file, s32 line)
{
    if (ptr == NULL)
    {
        return mem__malloc(size, file, line);
    }
    if (size == 0)
    {
        mem__free(ptr);
        return NULL;
    }

    struct mem_allocation *alloc = (struct mem_allocation *)ptr - 1;
    if (size <= alloc->size)
    {
        return ptr;
    }

    void *new_ptr = mem__malloc(size, file, line);
    if (new_ptr == NULL)
    {
        return NULL;
    }

    memcpy(new_ptr, ptr, alloc->size);
    mem__free(ptr);

    return new_ptr;
}

void *
mem__calloc(u32 num_items, u32 size, char *file, s32 line)
{
    if (num_items == 0 || size == 0)
    {
        return NULL;
    }
    if (math_ceil_log2(num_items) + math_ceil_log2(size) >= 32)
    {
        return NULL;
    }

    void *new_ptr = mem__malloc(num_items * size, file, line);
    if (new_ptr == NULL)
    {
        return NULL;
    }

    memset(new_ptr, 0, num_items * size);
    return new_ptr;
}

char *
mem__strdup(char *str, char *file, s32 line)
{
    char *ptr = mem__malloc(strlen(str) + 1, file, line);
    if (ptr == NULL)
    {
        return NULL;
    }
    strcpy(ptr, str);
    return ptr;
}

void
mem_dump()
{
    struct mem_allocation *alloc = allocation_head;
    while (alloc != NULL)
    {
        if ((ptrdiff_t)alloc->size >= 0)
        {
            printf("LEAKED: %s (%4d): %8d bytes at %p\n",
                   alloc->file,
                   alloc->line,
                   (s32)alloc->size,
                   (void *)(alloc + 1));
        }
    }
}

#define malloc(size) mem__malloc(size, __FILE__, __LINE__)
#define realloc(ptr, size) mem__realloc(ptr, size, __FILE__, __LINE__)
#define calloc(num_items, size) mem__calloc(num_items, size, __FILE__, __LINE__)
#define free(ptr) mem__free(ptr)
#define strdup(ptr) mem__strdup(ptr, __FILE__, __LINE__)
