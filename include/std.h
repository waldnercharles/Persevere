#ifndef STD_H
#define STD_H

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include <GL/glew.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#else
#define stat _stat
#endif

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef float r32;
typedef double r64;
typedef bool b32;

#define unused(x) (void)x

#define kilobytes(x) (x * 1024L)
#define megabytes(x) (kilobytes(x) * 1024L)
#define gigabytes(x) (megabytes(x) * 1024L)

#define export __declspec(dllexport)

#endif