// include/logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

void init_logger(const char *log_file);
void log_info(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_debug(const char *fmt, ...);
void close_logger(void);

#endif // LOGGER_H