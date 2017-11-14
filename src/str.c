#include <stdlib.h>
#include <string.h>

#include "str.h"

static void
string_set_length(char *str, u32 len)
{
    string_get_header(str)->len = len;
}

static void
string_set_capacity(char *str, u32 capacity)
{
    string_get_header(str)->capacity = capacity;
}

static void *
string_realloc(void *ptr, u32 old_size, u32 new_size)
{
    void *new_ptr;

    if (!ptr)
    {
        return malloc(new_size);
    }

    if (new_size < old_size)
    {
        new_size = old_size;
    }

    if (old_size == new_size)
    {
        return ptr;
    }

    new_ptr = malloc(new_size);
    if (!new_ptr)
    {
        return NULL;
    }

    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
}

char *
string_new_len(void *init_str, u32 len)
{
    struct string_header *header;
    u32 header_size;
    char *str;
    void *ptr;

    header_size = sizeof(struct string_header);
    ptr = malloc(header_size + len + 1);

    if (ptr == NULL)
    {
        return NULL;
    }
    if (!init_str)
    {
        memset(ptr, 0, header_size + len + 1);
    }

    str = (char *)ptr + header_size;
    header = string_get_header(str);
    header->len = len;
    header->capacity = len;
    if (len && init_str)
    {
        memcpy(str, init_str, len);
    }
    str[len] = '\0';

    return str;
}

char *
string_new(char *init_str)
{
    u32 len = init_str ? strlen(init_str) : 0;
    return string_new_len(init_str, len);
}

void
string_free(char *str)
{
    if (str == NULL)
    {
        return;
    }
    free((struct string_header *)str - 1);
}

char *
string_dup(char *str)
{
    return string_new_len(str, string_get_length(str));
}

u32
string_get_length(char *str)
{
    return string_get_header(str)->len;
}

u32
string_get_capacity(char *str)
{
    return string_get_header(str)->capacity;
}

u32
string_get_available_space(char *str)
{
    struct string_header *header = string_get_header(str);
    if (header->capacity > header->len)
    {
        return header->capacity - header->len;
    }
    return 0;
}

u32
string_get_allocated_size(char *str)
{
    u32 capacity = string_get_capacity(str);
    return sizeof(struct string_header) + capacity;
}

void
string_clear(char *str)
{
    string_set_length(str, 0);
    str[0] = '\0';
}

char *
string_append_len(char *str, void *other, u32 other_len)
{
    u32 curr_len = string_get_length(str);

    str = string_grow(str, other_len);
    if (str == NULL)
    {
        return NULL;
    }

    memcpy(str + curr_len, other, other_len);
    str[curr_len + other_len] = '\0';
    string_set_length(str, curr_len + other_len);

    return str;
}

char *
string_append(char *str, char *other)
{
    return string_append_len(str, other, string_get_length(other));
}

char *
string_append_cstring(char *str, char *other)
{
    return string_append_len(str, other, strlen(other));
}

char *
string_assign(char *str, char *cstr)
{
    u32 len = strlen(cstr);
    if (string_get_capacity(str) < len)
    {
        str = string_grow(str, len - string_get_length(str));
        if (str == NULL)
            return NULL;
    }

    memcpy(str, cstr, len);
    str[len] = '\0';
    string_set_length(str, len);

    return str;
}

char *
string_grow(char *str, u32 add_len)
{
    void *ptr, *new_ptr;
    u32 len, new_len, available, old_size, new_size;

    len = string_get_length(str);
    new_len = len + add_len;
    available = string_get_available_space(str);
    if (available >= add_len)
    {
        return str;
    }

    ptr = (char *)str - sizeof(struct string_header);
    old_size = sizeof(struct string_header) + string_get_length(str) + 1;
    new_size = sizeof(struct string_header) + new_len + 1;

    new_ptr = string_realloc(ptr, old_size, new_size);
    if (new_ptr == NULL)
    {
        return NULL;
    }

    str = (char *)new_ptr + sizeof(struct string_header);
    string_set_capacity(str, new_len);

    return str;
}

s32
string_equals(char *lhs, char *rhs)
{
    u32 i;
    u32 lhs_len = string_get_length(lhs);
    u32 rhs_len = string_get_length(rhs);

    if (lhs_len != rhs_len)
    {
        return 0;
    }

    for (i = 0; i < lhs_len; i++)
    {
        if (lhs[i] != rhs[i])
        {
            return 0;
        }
    }

    return 1;
}
