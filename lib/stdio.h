#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>

void PRINT(const char *format, ...);
void READ(const char *format, ...);
int uart_atoi(const char *s);
int32_t uart_atof(const char *str);

#endif