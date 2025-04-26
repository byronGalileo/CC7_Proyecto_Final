#ifndef OS_H
#define OS_H
#include <stdint.h>

void uart_putc(char c);
char uart_getc();
void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);
int uart_atoi(const char *s);
void uart_itoa(int num, char *buffer);
int32_t uart_atof(const char *str);
void uart_ftoa(float f, char *buffer, int precision);
void delay_loop();

#endif // OS_H
