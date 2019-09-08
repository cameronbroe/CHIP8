//
// Created by cameron on 9/8/2019.
//

#include "log.h"

FILE* log_file;

void log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char new_format[1024];
    strcpy(new_format, "INFO: ");
    strcat(new_format, format);
    vfprintf(log_file, new_format, args);
    va_end(args);
}

void log_debug(const char *format, ...) {
#ifdef DEBUG
    va_list args;
    va_start(args, format);

    char new_format[1024];
    strcpy(new_format, "DEBUG: ");
    strcat(new_format, format);
    vfprintf(log_file, new_format, args);
    va_end(args);
#endif
}

void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char new_format[1024];
    strcpy(new_format, "ERROR: ");
    strcat(new_format, format);
    vfprintf(log_file, new_format, args);
    va_end(args);
}

void log_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char new_format[1024];
    strcpy(new_format, "WARNING: ");
    strcat(new_format, format);
    vfprintf(log_file, new_format, args);
    va_end(args);
}

void initialize_logger() {
    log_file = fopen("chip8.log", "w+");
}

void close_logger() {
    fclose(log_file);
}

