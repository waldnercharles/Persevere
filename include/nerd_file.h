#pragma once

#include "nerd.h"
#include "nerd_math.h"
#include "nerd_memory.h"

#define file_t FILE

size_t file_length(file_t *f);
int file_compare(char *filename1, char *filename2);
bool file_equals(char *filename1, char *filename2);
void file_copy(char *from, char *to);
bool file_exists(char *filename);
char *file_cstr(char *filename, size_t *length);
void *file_load(const char *filename, size_t *length);
