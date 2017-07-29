#ifndef COMMON_H
#define COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t        int8;
typedef int16_t       int16;
typedef int32_t       int32;
typedef int64_t       int64;
typedef uint8_t       uint8;
typedef uint16_t      uint16;
typedef uint32_t      uint32;
typedef uint64_t      uint64;
typedef unsigned int  uint;

////////////////////////////////////////////////////////////////////////////////
//				      MATH

#ifndef MATH_H
#define MATH_H

extern bool math_is_pow2(uint n);
extern int  math_floor_log2(uint n);
extern int  math_ceil_log2(uint n);

#endif

#ifndef MATH_IMPLEMENTATION
#define MATH_IMPLEMENTATION

#define math_min(a, b) ((a) < (b) ? (a) : (b))
#define math_max(a, b) ((a) > (b) ? (a) : (b))

bool math_is_pow2(uint n)
{
    return n && !(n & (n - 1));
}


int math_floor_log2(uint n)
{
    static char log2_4[16] = { -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };
    if (n < (1U << 14))
    {
	if (n < (1U <<  4))      return 0  + log2_4[n      ];
	else if (n < (1U <<  9)) return 5  + log2_4[n >>  5];
	else                     return 10 + log2_4[n >> 10];
    }
    else if (n < (1U << 24))
    {
	if (n < (1U << 19))      return 15 + log2_4[n >> 15];
	else                     return 20 + log2_4[n >> 20];
    }
    else if (n < (1U << 29))     return 25 + log2_4[n >> 25];
    else                         return 30 + log2_4[n >> 30];
}


int math_ceil_log2(uint n)
{
    if (math_is_pow2(n)) return math_floor_log2(n);
    else return math_floor_log2(n) + 1;
}

#endif

////////////////////////////////////////////////////////////////////////////////
//				     MEMORY

#ifndef MEMORY_H
#define MEMORY_H

extern void *mem__malloc(size_t size, char *file, int line);
extern void  mem__free(void *ptr);
extern void *mem__realloc(void *ptr, size_t size, char *file, int line);
extern void *mem__calloc(size_t num_items, size_t size, char *file, int line);
extern char *mem__strdup(char *str, char *file, int line);
extern void  mem_dump();

/* include anything that uses memory allocation before macros */
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#endif

#ifndef MEMORY_IMPLEMENTATION
#define MEMORY_IMPLEMENTATION

#undef malloc
#undef realloc
#undef free
#undef calloc
#undef strdup

typedef struct mem_allocation
{
    char *file;
    int line;
    size_t size;
    struct mem_allocation *prev, *next;
} mem_allocation;

static mem_allocation *allocation_head;

int mem__alloc_count;


int mem__malloc_failure;

void *mem__malloc(size_t size, char *file, int line)
{
    if (size == 0) return NULL;

    mem_allocation *alloc = (mem_allocation *)malloc(size + sizeof(*alloc));
    if (alloc == NULL) return NULL;

    alloc->file = file;
    alloc->line = line;
    alloc->next = allocation_head;
    if (allocation_head != NULL) alloc->next->prev = alloc;
    
    alloc->prev = NULL;
    alloc->size = (int)size;

    allocation_head = alloc;
    return alloc + 1;
}


void mem__free(void *ptr)
{
    if (ptr == NULL) return;
    mem_allocation *alloc = (mem_allocation *)ptr - 1;

    if (alloc->next != NULL) alloc->next->prev = alloc->prev;
    if (alloc->prev != NULL) alloc->prev->next = alloc->next;
    else allocation_head = alloc->next;

    free(ptr);
}


void *mem__realloc(void *ptr, size_t size, char *file, int line)
{
    if (ptr == NULL) return mem__malloc(size, file, line);
    if (size == 0)
    {
	mem__free(ptr);
	return NULL;
    }
    
    mem_allocation *alloc = (mem_allocation *)ptr - 1;
    if (size <= alloc->size) return ptr;
    
    void *new_ptr = mem__malloc(size, file, line);
    if (new_ptr == NULL) return NULL;

    memcpy(new_ptr, ptr, alloc->size);
    mem__free(ptr);

    return new_ptr;
}


void *mem__calloc(size_t num_items, size_t size, char *file, int line)
{
    if (num_items == 0 || size == 0) return NULL;
    if (math_ceil_log2(num_items) + math_ceil_log2(size) >= 32) return NULL;

    void *new_ptr = mem__malloc(num_items * size, file, line);
    if (new_ptr == NULL) return NULL;

    memset(new_ptr, 0, num_items * size);
    return new_ptr;
}


char *mem__strdup(char *str, char *file, int line)
{
    char *ptr = (char *)mem__malloc(strlen(str) + 1, file, line);
    if (ptr == NULL) return NULL;
    strcpy(ptr, str);
    return ptr;
}


void mem_dump()
{
    mem_allocation *alloc = allocation_head;
    while (alloc != NULL)
    {
	if ((ptrdiff_t)alloc->size >= 0)
	    printf("LEAKED: %s (%4d): %8d bytes at %p\n",
		   alloc->file,
		   alloc->line,
		   (int)alloc->size,
		   (void *)(alloc + 1));
    }
}


#define malloc(size)            mem__malloc(size, __FILE__, __LINE__)
#define realloc(ptr, size)      mem__realloc(ptr, size, __FILE__, __LINE__)
#define calloc(num_items, size) mem__calloc(num_items, size, __FILE__, __LINE__)
#define free(ptr)               mem__free(ptr)
#define strdup(ptr)             mem__strdup(ptr, __FILE__, __LINE__)

#endif




////////////////////////////////////////////////////////////////////////////////
//				     STRING

#ifndef STRING_H
#define STRING_H

#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>

typedef struct string_header
{
    size_t len;
    size_t capacity;
} string_header;


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


#define string_get_header(s) ((string_header *)s - 1)


static void string_set_length(char *str, size_t len)
{
    string_get_header(str)->len = len;
}


static void string_set_capacity(char *str, size_t capacity)
{
    string_get_header(str)->capacity = capacity;
}


static void *string_realloc(void *ptr, size_t old_size, size_t new_size)
{
    if (!ptr) return malloc(new_size);
    if (new_size < old_size) new_size = old_size;
    if (old_size == new_size) return ptr;

    void *new_ptr = malloc(new_size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
}


char *string_new_len(const void *init_str, size_t len)
{
    size_t header_size = sizeof(string_header);
    void *ptr = malloc(header_size + len + 1);

    if (ptr == NULL) return NULL;
    if (!init_str)
    {
	memset(ptr, 0, header_size + len + 1);
    }

    char *str = (char *)ptr + header_size;
    string_header *header = string_get_header(str);
    header->len = len;
    header->capacity = len;
    if (len && init_str)
    {
	memcpy(str, init_str, len);
    }
    str[len] = '\0';

    return str;
}


char *string_new(char const *init_str)
{
    size_t len = init_str ? strlen(init_str) : 0;
    return string_new_len(init_str, len);
}


void string_free(char *str)
{
    if (str == NULL) return;
    free((string_header *)str - 1);
}


char *string_dup(char *const str)
{
    return string_new_len(str, string_get_length(str));
}


size_t string_get_length(char *const str)
{
    return string_get_header(str)->len;
}


size_t string_get_capacity(char *const str)
{
    return string_get_header(str)->capacity;
}


size_t string_get_available_space(char *const str)
{
    string_header *header = string_get_header(str);
    if (header->capacity > header->len)
    {
	return header->capacity - header->len;
    }
    return 0;
}


size_t string_get_allocated_size(char *const str)
{
    size_t capacity = string_get_capacity(str);
    return sizeof(string_header) + capacity;
}


void string_clear(char *str)
{
    string_set_length(str, 0);
    str[0] = '\0';
}


char *string_append_len(char *str, void const *other, size_t other_len)
{
    size_t curr_len = string_get_length(str);

    str = string_grow(str, other_len);
    if (str == NULL) return NULL;

    memcpy(str + curr_len, other, other_len);
    str[curr_len + other_len] = '\0';
    string_set_length(str, curr_len + other_len);

    return str;
}


char *string_append(char *str, char *const other)
{
    return string_append_len(str, other, string_get_length(other));
}


char *string_append_cstring(char *str, char const *other)
{
    return string_append_len(str, other, strlen(other));
}


char *string_assign(char *str, char const *cstr)
{
    size_t len = strlen(cstr);
    if (string_get_capacity(str) < len)
    {
	str = string_grow(str, len - string_get_length(str));
	if (str == NULL) return NULL;
    }

    memcpy(str, cstr, len);
    str[len] = '\0';
    string_set_length(str, len);

    return str;
}


char *string_grow(char *str, size_t add_len)
{
    size_t len = string_get_length(str);
    size_t new_len = len + add_len;
    size_t available = string_get_available_space(str);
    if (available >= add_len) return str;

    void *ptr = (char *)str - sizeof(string_header);
    size_t old_size = sizeof(string_header) + string_get_length(str) + 1;
    size_t new_size = sizeof(string_header) + new_len + 1;

    void *new_ptr = string_realloc(ptr, old_size, new_size);
    if (new_ptr == NULL) return NULL;

    str = (char *)new_ptr + sizeof(string_header);
    string_set_capacity(str, new_len);

    return str;
}


int string_equals(char *const lhs, char *const rhs)
{
    size_t lhs_len = string_get_length(lhs);
    size_t rhs_len = string_get_length(rhs);
    if (lhs_len != rhs_len) return 0;

    for (int i = 0; i < lhs_len; i++)
    {
	if (lhs[i] != rhs[i])
	{
	    return 0;
	}
    }

    return 1;
}


#endif


////////////////////////////////////////////////////////////////////////////////
//				      FILE

#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define stat _stat
#endif

#define file_t FILE

extern size_t file_length(file_t *f);
extern int    file_compare(char *fname1, char *fname2);
extern bool   file_equals(char *fname1, char *fname2);
extern bool   file_exists(char *f);
extern file_t file_open(char *f);
extern time_t file_timestamp(char *f);

#endif

#ifndef FILE_IMPLEMENTATION
#define FILE_IMPLEMENTATION

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

#endif 

#endif // COMMON_H
