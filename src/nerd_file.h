#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "nerd_math.h"
#include "nerd_memory.h"

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define stat _stat
#endif

#define file_t FILE

static int file_compare_internal(file_t *f1, file_t *f2);

size_t file_length(file_t *f)
{
    size_t pos = ftell(f);
    fseek(f, 0, SEEK_END);

    size_t len = ftell(f);
    fseek(f, pos, SEEK_SET);

    return len;
}


int file_compare(char *fname1, char *fname2)
{
    file_t *f1 = fopen(fname1, "rb");
    file_t *f2 = fopen(fname2, "rb");

    if (f1 == NULL || f2 == NULL)
    {
	if (f1 != NULL)
	{
	    fclose(f1);
	    return 1;
	}
	if (f2 != NULL)
	{
	    fclose(f2);
	    return -1;
	}
	return 0;
    }

    return file_compare_internal(f1, f2);
}


bool file_equals(char *fname1, char *fname2)
{
    file_t *f1 = fopen(fname1, "rb");
    file_t *f2 = fopen(fname2, "rb");

    if (f1 == NULL || f2 == NULL)
    {
	if (f1 != NULL) fclose(f1);
	if (f2 != NULL) fclose(f2);
	return f1 == f2;
    }

    if (file_length(f1) != file_length(f2))
    {
	fclose(f1);
	fclose(f2);
	return 0;
    }

    return !file_compare_internal(f1, f2);
}


void file_copy(char *from, char *to)
{
    char buf[2048];
    size_t nread;

    file_t *f1 = fopen(from, "rb");
    file_t *f2 = fopen(to, "wb");

    if (f1 == NULL || f2 == NULL)
    {
	if (f1 != NULL) fclose(f1);
	if (f2 != NULL) fclose(f2);
	return;
    }

    while ((nread = fread(buf, 1, sizeof(buf), f1)))
    {
	fwrite(buf, 1, sizeof(buf), f2);
    }
    fclose(f1);
    fclose(f2);
}


bool file_exists(char *fname)
{
    struct stat buf;
    return stat(fname, &buf) == 0;
}


char *file_cstr(char *fname, size_t *length)
{
    file_t *f = fopen(fname, "rb");
    char *buf;
    size_t len, read_len;

    if (f == NULL) return NULL;

    len = file_length(f);
    buf = (char *)malloc(len+1);
    read_len = fread(buf, 1, len, f);

    if (read_len == len)
    {
	if (length) *length = len;
	buf[len] = 0;
    }
    else
    {
	free(buf);
	buf = NULL;
    }

    fclose(f);
    return buf;
}


static int file_compare_internal(file_t *f1, file_t *f2)
{
    char buf1[2048], buf2[2048];
    int cmp = 0;
    while (1)
    {
	size_t size1 = fread(buf1, 1, sizeof(buf1), f1);
	size_t size2 = fread(buf2, 1, sizeof(buf2), f2);
	cmp = memcmp(buf1, buf2, math_min(size1, size2));

	if (cmp != 0) break;
	if (size1 != size2)
	{
	    cmp = size1 < size2 ? -1 : 1;
	    break;
	}
	if (size1 == 0) break;
    }

    return cmp;
}
