#ifndef FILE_H
#define FILE_H
#include "std.h"
#include "allocators/allocator.h"

u32 file_length(FILE *f);
s32 file_compare(const char *filename1, const char *filename2);
b32 file_equals(const char *filename1, const char *filename2);
void file_copy(const char *from, const char *to);
b32 file_exists(const char *filename);

char *file_load_cstr(struct allocator *allocator,
                     const char *filename,
                     u32 *length);

void *file_load(struct allocator *allocator, const char *filename, u32 *length);
time_t file_write_time(char *filename);

#endif