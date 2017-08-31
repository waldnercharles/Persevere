#pragma once

#include "nerd.h"
#include "nerd_memory.h"

struct string_header
{
    size_t len;
    size_t capacity;
};

char *string_new(char const *init_str);
char *string_new_len(void const *init_str, size_t len);
void string_free(char *str);

char *string_dup(char *const str);

size_t string_get_length(char *const str);
size_t string_get_capacity(char *const str);
size_t string_get_available_space(char *const str);
size_t string_get_allocated_size(char *const str);

void string_clear(char *str);

char *string_append_len(char *str, void const *other, size_t len);
char *string_append(char *str, char *const other);
char *string_append_cstring(char *str, char const *other);

char *string_assign(char *str, char const *cstr);

char *string_grow(char *str, size_t add_len);

int string_strings_are_equal(char *const lhs, char *const rhs);

#define string_get_header(s) ((struct string_header *)s - 1)