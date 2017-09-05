#pragma once

#include "nerd.h"
#include "nerd_math.h"
#include "nerd_memory.h"

#define file_t FILE

size_t file_length(file_t *f);
int file_compare(const char *filename1, const char *filename2);
bool file_equals(const char *filename1, const char *filename2);
void file_copy(const char *from, const char *to);
bool file_exists(const char *filename);
char *file_load_cstr(const char *filename, size_t *length);
void *file_load(const char *filename, size_t *length);
