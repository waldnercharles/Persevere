#pragma once

#include "nerd.h"
#include "nerd_math.h"
#include "nerd_memory.h"

#define file_t FILE

static int file__compare(file_t *f1, file_t *f2);

size_t file_length(file_t *f)
{
    size_t pos = ftell(f);
    fseek(f, 0, SEEK_END);

    size_t len = ftell(f);
    fseek(f, pos, SEEK_SET);

    return len;
}

int file_compare(char *filename1, char *filename2)
{
    file_t *f1 = fopen(filename1, "rb");
    file_t *f2 = fopen(filename2, "rb");

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

    return file__compare(f1, f2);
}

bool file_equals(char *filename1, char *filename2)
{
    file_t *f1 = fopen(filename1, "rb");
    file_t *f2 = fopen(filename2, "rb");

    if (f1 == NULL || f2 == NULL)
    {
        if (f1 != NULL)
        {
            fclose(f1);
        }
        if (f2 != NULL)
        {
            fclose(f2);
        }
        return f1 == f2;
    }

    if (file_length(f1) != file_length(f2))
    {
        fclose(f1);
        fclose(f2);
        return 0;
    }

    return !file__compare(f1, f2);
}

void file_copy(char *from, char *to)
{
    char buf[2048];
    size_t nread;

    file_t *f1 = fopen(from, "rb");
    file_t *f2 = fopen(to, "wb");

    if (f1 == NULL || f2 == NULL)
    {
        if (f1 != NULL)
        {
            fclose(f1);
        }
        if (f2 != NULL)
        {
            fclose(f2);
        }
        return;
    }

    while ((nread = fread(buf, 1, sizeof(buf), f1)))
    {
        fwrite(buf, 1, sizeof(buf), f2);
    }
    fclose(f1);
    fclose(f2);
}

bool file_exists(char *filename)
{
    struct stat buf;
    return stat(filename, &buf) == 0;
}

static void *file__load(const char *filename, size_t *length, size_t additional_length)
{
    file_t *f = fopen(filename, "rb");
    size_t len, read_len;
    void *buf;

    if (f == NULL)
    {
        return NULL;
    }

    len = file_length(f);
    buf = malloc(len + additional_length);
    if (buf == NULL)
    {
        fclose(f);
        return NULL;
    }

    read_len = fread(buf, 1, len, f);
    fclose(f);

    if (read_len == len)
    {
        if (length != NULL)
        {
            *length = len;
        }
    }
    else
    {
        free(buf);
        return NULL;
    }

    return buf;
}

char *file_cstr(char *filename, size_t *length)
{
    char *buf;
    size_t len;
    if (length)
    {
        buf = file__load(filename, length, 1);
        buf[*length] = 0;
    }
    else
    {
        buf = file__load(filename, &len, 1);
        buf[len] = 0;
    }

    return buf;
}

void *file_load(const char *filename, size_t *length) { return file__load(filename, length, 0); }

static int file__compare(file_t *f1, file_t *f2)
{
    char buf1[2048], buf2[2048];
    size_t size1, size2;
    int cmp = 0;
    while (1)
    {
        size1 = fread(buf1, 1, sizeof(buf1), f1);
        size2 = fread(buf2, 1, sizeof(buf2), f2);
        cmp = memcmp(buf1, buf2, math_min(size1, size2));

        if (cmp != 0)
        {
            break;
        }
        if (size1 != size2)
        {
            cmp = size1 < size2 ? -1 : 1;
            break;
        }
        if (size1 == 0)
        {
            break;
        }
    }
    return cmp;
}
