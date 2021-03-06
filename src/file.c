#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#else
#define stat _stat
#endif

#include "file.h"
#include "log.h"
#include "vec.h"

static void *
file__load(struct allocator *allocator,
           const char *filename,
           u32 *length,
           u32 additional_length)
{
    void *buf;
    u32 len, read_len;
    FILE *f = fopen(filename, "rb");

    if (f == NULL)
    {
        log_error("Failed to load %s.", filename);
        return NULL;
    }

    len = file_length(f);
    buf = alloc(allocator, len + additional_length);
    if (buf == NULL)
    {
        fclose(f);
        log_error("Failed to allocate buffer of size %i",
                  len + additional_length);
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
        log_warning("Buffer read mismatch. %i out of %i bytes read.",
                    read_len,
                    len);
        // free(buf);
        return NULL;
    }

    return buf;
}

static s32
file__compare(FILE *f1, FILE *f2)
{
    u8 buf1[2048], buf2[2048];
    u32 size1, size2;
    s32 cmp = 0;
    while (1)
    {
        size1 = fread(buf1, 1, sizeof(buf1), f1);
        size2 = fread(buf2, 1, sizeof(buf2), f2);
        cmp = memcmp(buf1, buf2, u32_min(size1, size2));

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

u32
file_length(FILE *f)
{
    u32 pos, len;

    pos = ftell(f);
    fseek(f, 0, SEEK_END);

    len = ftell(f);
    fseek(f, pos, SEEK_SET);

    return len;
}

s32
file_compare(const char *filename1, const char *filename2)
{
    FILE *f1 = fopen(filename1, "rb");
    FILE *f2 = fopen(filename2, "rb");

    if (f1 == NULL || f2 == NULL)
    {
        if (f1 != NULL)
        {
            log_debug("Failed to open file %s.", filename1);
            fclose(f1);
            return 1;
        }
        if (f2 != NULL)
        {
            log_debug("Failed to open file %s.", filename2);
            fclose(f2);
            return -1;
        }
        return 0;
    }

    return file__compare(f1, f2);
}

b32
file_equals(const char *filename1, const char *filename2)
{
    FILE *f1 = fopen(filename1, "rb");
    FILE *f2 = fopen(filename2, "rb");

    if (f1 == NULL || f2 == NULL)
    {
        if (f1 != NULL)
        {
            log_debug("Failed to open file %s.", filename1);
            fclose(f1);
        }
        if (f2 != NULL)
        {
            log_debug("Failed to open file %s.", filename2);
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

void
file_copy(const char *from, const char *to)
{
    char buf[2048];
    u32 nread;

    FILE *f1 = fopen(from, "rb");
    FILE *f2 = fopen(to, "wb");

    if (f1 == NULL || f2 == NULL)
    {
        if (f1 != NULL)
        {
            log_debug("Failed to open file %s.", from);
            fclose(f1);
        }
        if (f2 != NULL)
        {
            log_debug("Failed to open file %s.", to);
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

b32
file_exists(const char *filename)
{
    struct stat buf;
    return stat(filename, &buf) == 0;
}

char *
file_load_cstr(struct allocator *allocator, const char *filename, u32 *length)
{
    char *buf;
    u32 len = 0;

    if (length)
    {
        buf = file__load(allocator, filename, length, 1);

        if (buf == NULL)
        {
            log_error("Failed to load %s.", filename);
        }

        buf[*length] = 0;
    }
    else
    {
        buf = file__load(allocator, filename, &len, 1);
        if (buf == NULL)
        {
            log_error("Failed to load %s.", filename);
        }

        buf[len] = 0;
    }

    return buf;
}

void *
file_load(struct allocator *allocator, const char *filename, u32 *length)
{
    return file__load(allocator, filename, length, 0);
}

time_t
file_write_time(char *filename)
{
    struct stat buf;
    return stat(filename, &buf) == 0 ? buf.st_mtime : 0;
}