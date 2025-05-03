#include <stdarg.h>
#include "os.h"
#include "stdio.h"

void to_string (int num, char *buffer) {
    // Convert int to string
    uart_itoa(num, buffer);
}

int to_int (char *s) {
    // Convert string to integer
    return uart_atoi(s);
}

// Bare-metal implementation of printf
void PRINT(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[32];
    char c;
    const char *str;

    // %d: Prints an integer in decimal format.
    // %x: Prints an integer in hexadecimal format.
    // %s: Prints a null-terminated string.
    // %c: Prints a single character.

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':  // Integer (decimal)
                    to_string(va_arg(args, int), buffer);
                    uart_puts(buffer);
                    break;
                case 'x':  // Integer (hexadecimal)
                    uart_itoa(va_arg(args, int), buffer);
                    uart_puts(buffer);
                    break;
                case 's':  // String
                    str = va_arg(args, const char *);
                    uart_puts(str);
                    break;
                case 'c':  // Character
                    c = (char)va_arg(args, int);
                    uart_putc(c);
                    break;
                case 'f': {  // Fixed-point (assumes input is scaled by 1000)
                    int32_t fixed_num = va_arg(args, int32_t);
                    char buffer[32];
                
                    // Handle negative numbers
                    if (fixed_num < 0) {
                        uart_putc('-');
                        fixed_num = -fixed_num;
                    }
                
                    // Integer part
                    int32_t int_part = fixed_num / 1000;
                    uart_itoa(int_part, buffer);
                    uart_puts(buffer);
                    uart_putc('.');
                
                    // Fractional part (3 digits)
                    int32_t frac_part = fixed_num % 1000;
                    if (frac_part < 100) uart_putc('0');  // Leading zero (e.g., "0.05" instead of "0.5")
                    if (frac_part < 10) uart_putc('0');
                    uart_itoa(frac_part, buffer);
                    uart_puts(buffer);
                    break;
                }
                default:  // Unsupported format specifier
                    uart_putc('%');
                    uart_putc(*format);
                    break;
            }
        } else {
            // Regular character
            uart_putc(*format);
        }
        format++;
    }

    va_end(args);
}

// Custom scanf-like function for UART
void READ(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buffer[64];  // Temporary buffer for input
    char *str_arg;
    int *int_arg;
    char *char_arg;
    int i = 0;

    while (fmt[i]) {
        if (fmt[i] == '%') {
            i++; // Move past '%'
            switch (fmt[i]) {
                case 'd':  // Integer
                    uart_gets_input(buffer, sizeof(buffer));  // Read input
                    int_arg = va_arg(args, int *);
                    *int_arg = uart_atoi(buffer);
                    break;

                case 's':  // String
                    str_arg = va_arg(args, char *);
                    uart_gets_input(str_arg, 64);  // Read input
                    break;
                case 'f': {  // Fixed-point (instead of float)
                    int32_t *fixed_arg = va_arg(args, int32_t *);
                    uart_gets_input(buffer, sizeof(buffer));
                    *fixed_arg = uart_atof(buffer);  // Stores as scaled integer (e.g., 3140 for "3.14")
                    break;
                }
                case 'c':  // Character
                    char_arg = va_arg(args, char *);
                    *char_arg = uart_getc();
                    uart_putc(*char_arg); // Echo the character
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