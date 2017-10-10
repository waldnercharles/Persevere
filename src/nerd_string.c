#include "nerd_string.h"

static void
string_set_length(char *str, size_t len)
{
    string_get_header(str)->len = len;
}

static void
string_set_capacity(char *str, size_t capacity)
{
    string_get_header(str)->capacity = capacity;
}

static void *
string_realloc(void *ptr, size_t old_size, size_t new_size)
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
string_new_len(const void *init_str, size_t len)
{
    struct string_header *header;
    size_t header_size;
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
string_new(char const *init_str)
{
    size_t len = init_str ? strlen(init_str) : 0;
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
string_dup(char *const str)
{
    return string_new_len(str, string_get_length(str));
}

size_t
string_get_length(char *const str)
{
    return string_get_header(str)->len;
}

size_t
string_get_capacity(char *const str)
{
    return string_get_header(str)->capacity;
}

size_t
string_get_available_space(char *const str)
{
    struct string_header *header = string_get_header(str);
    if (header->capacity > header->len)
    {
        return header->capacity - header->len;
    }
    return 0;
}

size_t
string_get_allocated_size(char *const str)
{
    size_t capacity = string_get_capacity(str);
    return sizeof(struct string_header) + capacity;
}

void
string_clear(char *str)
{
    string_set_length(str, 0);
    str[0] = '\0';
}

char *
string_append_len(char *str, void const *other, size_t other_len)
{
    size_t curr_len = string_get_length(str);

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
string_append(char *str, char *const other)
{
    return string_append_len(str, other, string_get_length(other));
}

char *
string_append_cstring(char *str, char const *other)
{
    return string_append_len(str, other, strlen(other));
}

char *
string_assign(char *str, char const *cstr)
{
    size_t len = strlen(cstr);
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
string_grow(char *str, size_t add_len)
{
    void *ptr, *new_ptr;
    size_t len, new_len, available, old_size, new_size;

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

int
string_equals(char *const lhs, char *const rhs)
{
    size_t lhs_len = string_get_length(lhs);
    size_t rhs_len = string_get_length(rhs);

    if (lhs_len != rhs_len)
    {
        return 0;
    }

    for (size_t i = 0; i < lhs_len; i++)
    {
        if (lhs[i] != rhs[i])
        {
            return 0;
        }
    }

    return 1;
}
