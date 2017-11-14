#ifndef NERD_STRING_H
#define NERD_STRING_H
#include "nerd_engine.h"

struct string_header
{
    u32 len;
    u32 capacity;
};

char *string_new(char *init_str);
char *string_new_len(void *init_str, u32 len);
void string_free(char *str);

char *string_dup(char *str);

u32 string_get_length(char *str);
u32 string_get_capacity(char *str);
u32 string_get_available_space(char *str);
u32 string_get_allocated_size(char *str);

void string_clear(char *str);

char *string_append_len(char *str, void *other, u32 len);
char *string_append(char *str, char *other);
char *string_append_cstring(char *str, char *other);

char *string_assign(char *str, char *cstr);

char *string_grow(char *str, u32 add_len);

bool string_strings_are_equal(char *lhs, char *rhs);

#define string_get_header(s) ((struct string_header *)s - 1)
#endif