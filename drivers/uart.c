#include "uart.h"
#include "io.h"

#define UART0_BASE      0x44E09000
#define UART_THR        (UART0_BASE + 0x00)
#define UART_RHR        (UART0_BASE + 0x00)
#define UART_LSR        (UART0_BASE + 0x14)
#define UART_LSR_THRE   0x20
#define UART_LSR_DR     0x01

__attribute__((noinline))
void uart_putc(char c) {
    if (c == '\n') {
        while (!(GET32(UART_LSR) & UART_LSR_THRE));
        PUT32(UART_THR, '\r');
    }
    while (!(GET32(UART_LSR) & UART_LSR_THRE));
    PUT32(UART_THR, c);
}

char uart_getc(void) {
    while (!(GET32(UART_LSR) & UART_LSR_DR));
    return (char)GET32(UART_RHR);
}

__attribute__((noinline))
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}