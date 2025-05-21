#ifndef STRING_H
#define STRING_H

#include <stdint.h>

void uart_itoa(int num, char *buffer);
int uart_atoi(const char *s);
int32_t uart_atof(const char *str);
void int_to_str(int num, char *buffer);
void to_string(int num, char *buffer);

#endif