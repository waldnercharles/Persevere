#ifndef NERD_LOG_H
#define NERD_LOG_H
#include "nerd_engine.h"

#define log_level_none 0
#define log_level_error 1
#define log_level_warning 2
#define log_level_debug 3

#define log_error_tag "ERROR"
#define log_warning_tag "WARNING"
#define log_debug_tag "DEBUG"

#ifndef log_level
#define log_level log_level_debug
#endif

#define log_newline "\n"
#define log_fmt "[%s] (%s:%s:%i) "
#define log_args(tag) tag, __FILE__, __FUNCTION__, __LINE__

#if log_level >= log_level_error
#define log_error(message, ...)                                                \
    fprintf(stderr,                                                            \
            log_fmt message log_newline,                                       \
            log_args(log_error_tag),                                           \
            ##__VA_ARGS__),                                                    \
        exit(EXIT_FAILURE)
#else
#define error(message, ...)
#endif

#if log_level >= log_level_warning
#define log_warning(message, ...)                                              \
    fprintf(stdout,                                                            \
            log_fmt message log_newline,                                       \
            log_args(log_warning_tag),                                         \
            ##__VA_ARGS__)
#else
#define warning(message, ...)
#endif

#if log_level >= log_level_debug
#define log_debug(message, ...)                                                \
    fprintf(stdout,                                                            \
            log_fmt message log_newline,                                       \
            log_args(log_debug_tag),                                           \
            ##__VA_ARGS__)
#else
#define debug(message, ...)
#endif

#endif