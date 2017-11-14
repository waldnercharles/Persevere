#ifndef NERD_FILE_H
#define NERD_FILE_H
#include "nerd_engine.h"

u32 file_length(FILE *f);
s32 file_compare(const char *filename1, const char *filename2);
bool file_equals(const char *filename1, const char *filename2);
void file_copy(const char *from, const char *to);
bool file_exists(const char *filename);
char *file_load_cstr(const char *filename, u32 *length);
void *file_load(const char *filename, u32 *length);
#endif