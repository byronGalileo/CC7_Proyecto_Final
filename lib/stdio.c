#include <stdarg.h>
#include "../drivers/io.h"
#include "../drivers/uart.h"
#include "stdio.h"
#include "string.h"
#include "console.h"

void PRINT(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[32];
    char c;
    const char *str;
    const char *hex = "0123456789ABCDEF";

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                    to_string(va_arg(args, int), buffer);
                    uart_puts(buffer);
                    break;
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    uart_puts("0x");
                    for (int i = 28; i >= 0; i -= 4)
                        uart_putc(hex[(num >> i) & 0xF]);
                    break;
                }
                case 's':
                    str = va_arg(args, const char *);
                    uart_puts(str);
                    break;
                case 'c':
                    c = (char)va_arg(args, int);
                    uart_putc(c);
                    break;
                case 'f': {
                    int32_t fixed_num = va_arg(args, int32_t);
                    if (fixed_num < 0) {
                        uart_putc('-');
                        fixed_num = -fixed_num;
                    }
                    int32_t int_part = fixed_num / 1000;
                    uart_itoa(int_part, buffer);
                    uart_puts(buffer);
                    uart_putc('.');
                    int32_t frac_part = fixed_num % 1000;
                    if (frac_part < 100) uart_putc('0');
                    if (frac_part < 10) uart_putc('0');
                    uart_itoa(frac_part, buffer);
                    uart_puts(buffer);
                    break;
                }
                default:
                    uart_putc('%');
                    uart_putc(*format);
                    break;
            }
        } else {
            uart_putc(*format);
        }
        format++;
    }

    va_end(args);
}

void READ(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buffer[64];
    char *str_arg;
    int *int_arg;
    char *char_arg;
    int i = 0;

    while (fmt[i]) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 'd':
                    uart_gets_input(buffer, sizeof(buffer));
                    int_arg = va_arg(args, int *);
                    *int_arg = uart_atoi(buffer);
                    break;
                case 's':
                    str_arg = va_arg(args, char *);
                    uart_gets_input(str_arg, 64);
                    break;
                case 'f': {
                    int32_t *fixed_arg = va_arg(args, int32_t *);
                    uart_gets_input(buffer, sizeof(buffer));
                    *fixed_arg = uart_atof(buffer);
                    break;
                }
                case 'c':
                    char_arg = va_arg(args, char *);
                    *char_arg = uart_getc();
                    uart_putc(*char_arg);
                    break;
                default:
                    uart_puts("Unsupported format specifier\n");
                    break;
            }
        }
        i++;
    }

    va_end(args);
}