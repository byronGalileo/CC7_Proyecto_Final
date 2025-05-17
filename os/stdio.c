#include <stdarg.h>
#include "stdio.h"

// Function to send a single character via UART
void uart_putc(char c) {
    // Wait until Transmit Holding Register is empty
    while (!(GET32(UART_LSR) & UART_LSR_THRE));
    
    // Write character to Transmit Holding Register
    PUT32(UART_THR, c);
}

// Function to receive a single character from UART
char uart_getc() {
    // Wait until data is available in the Receive Holding Register
    while (!(GET32(UART_LSR) & UART_LSR_DR));

    // Read character from Receive Holding Register
    return (char)GET32(UART_RHR);
}

// Function to send a string via UART
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

// Function to receive a line of input via UART
void uart_gets_input(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) {
        c = uart_getc();

        // Handle newline and carriage return (consume both)
        if (c == '\r' || c == '\n') {
            uart_putc('\n'); // Echo newline
            // Also consume the other part of CRLF if it exists
            char next = uart_getc();
            if ((c == '\r' && next != '\n') || (c == '\n' && next != '\r')) {
                // If not a pair, push it back (optional)
            }
            break;
        }

        if (c == 0x08 || c == 0x7F) { // Backspace or DEL
            if (i > 0) {
                uart_putc('\b');
                uart_putc(' ');
                uart_putc('\b');
                i--;
            }
        } else {
            uart_putc(c);
            buffer[i++] = c;
        }
    }
    buffer[i] = '\0';
}

// Simple function to convert string to integer
int uart_atoi(const char *s) {
    int num = 0;
    int sign = 1;
    int i = 0;

    // Handle optional sign
    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num * 10 + (s[i] - '0');
    }

    return sign * num;
}

// Function to convert integer to string
void uart_itoa(int num, char *buffer) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0 && i < 14) { // Reserve space for sign and null terminator
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse the string
    int start = 0, end = i - 1;
    char temp;
    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

// Convert string to float
// Parses a string into a fixed-point integer (e.g., "3.14" -> 3140)
int32_t uart_atof(const char *s) {
    int32_t result = 0, fraction = 0;
    int32_t sign = 1, i = 0, decimal_found = 0;
    int32_t divisor = 1;

    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    while (s[i]) {
        if (s[i] == '.') {
            decimal_found = 1;
            i++;
            continue;
        }

        if (s[i] >= '0' && s[i] <= '9') {
            if (decimal_found) {
                fraction = fraction * 10 + (s[i] - '0');
                divisor *= 10;
            } else {
                result = result * 10 + (s[i] - '0');
            }
        } else {
            break;
        }
        i++;
    }

    // Combine integer + fractional parts (e.g., "3.14" -> 3140)
    return sign * (result * 1000 + (fraction * 1000 / divisor));
}

// Function to print a float
void uart_ftoa(float num, char *buffer, int precision) {
    if (num < 0) {
        *buffer++ = '-';
        num = -num;
    }

    int integerPart = (int)num;
    float fractionalPart = num - (float)integerPart;

    // Convert integer part to string
    uart_itoa(integerPart, buffer);

    // Find end of string
    while (*buffer) buffer++;

    // Add decimal point
    *buffer++ = '.';

    // Convert fractional part
    for (int j = 0; j < precision; j++) {
        fractionalPart *= 10;
        int digit = (int)fractionalPart;
        *buffer++ = '0' + digit;
        fractionalPart -= digit;
    }

    *buffer = '\0';
}

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
    const char *hex = "0123456789ABCDEF"; // Hexadecimal digits

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
                    unsigned int num = va_arg(args, unsigned int);
                    uart_puts("0x");
                    for (int i = 28; i >= 0; i -= 4) {
                        uart_putc(hex[(num >> i) & 0xF]);
                    }
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