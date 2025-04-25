// src/logger.c
#include "../../include/logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static FILE *log_file = NULL;

void init_logger(const char *log_file_path) {
    if (log_file_path) {
        log_file = fopen(log_file_path, "a");
        if (!log_file) {
            fprintf(stderr, "Cannot open log file %s\n", log_file_path);
        }
    }
}

void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

    printf("[%s] [INFO] ", time_str);
    vprintf(fmt, args);
    if (log_file) {
        fprintf(log_file, "[%s] [INFO] ", time_str);
        vfprintf(log_file, fmt, args);
        fflush(log_file);
    }
    va_end(args);
}

void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

    fprintf(stderr, "[%s] [ERROR] ", time_str);
    vfprintf(stderr, fmt, args);
    if (log_file) {
        fprintf(log_file, "[%s] [ERROR] ", time_str);
        vfprintf(log_file, fmt, args);
        fflush(log_file);
    }
    va_end(args);
}

void log_debug(const char *fmt, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

    printf("[%s] [DEBUG] ", time_str);
    vprintf(fmt, args);
    if (log_file) {
        fprintf(log_file, "[%s] [DEBUG] ", time_str);
        vfprintf(log_file, fmt, args);
        fflush(log_file);
    }
    va_end(args);
#endif
}

void close_logger(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}