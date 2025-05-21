#ifndef uart.h
#define uart.h

#include <stdint.h>

#define UART0_BASE      0x44E09000
#define UART_THR        (UART0_BASE + 0x00)
#define UART_RHR        (UART0_BASE + 0x00)  // Receive Holding Register
#define UART_LSR        (UART0_BASE + 0x14)
#define UART_LSR_THRE   0x20
#define UART_LSR_DR  (1 << 0)  // Data Ready

extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);
void uart_putc(char c);
char uart_getc();
void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);
int uart_atoi(const char *s);
void uart_itoa(int num, char *buffer);
int32_t uart_atof(const char *str);
void uart_ftoa(float f, char *buffer, int precision);

#endif