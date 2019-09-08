//
// Created by cameron on 9/8/2019.
//

#ifndef CHIP8_LOG_H
#define CHIP8_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void initialize_logger();
void log_info(const char* format, ...);
void log_debug(const char* format, ...);
void log_error(const char* format, ...);
void log_warning(const char* format, ...);
void close_logger();

#endif //CHIP8_LOG_H
