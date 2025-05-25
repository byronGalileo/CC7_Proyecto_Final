#include "os.h"

extern void PUT32(unsigned int addr, unsigned int value);
extern unsigned int GET32(unsigned int addr);

#define UART0_BASE  0x44E09000  // Example for AM335x
#define UART_THR    (UART0_BASE + 0x00)  // Transmit Holding Register
#define UART_RHR    (UART0_BASE + 0x00)  // Receive Holding Register
#define UART_LSR    (UART0_BASE + 0x14)  // Line Status Register
#define UART_LSR_THRE (1 << 5)  // Transmit Holding Register Empty
#define UART_LSR_DR  (1 << 0)  // Data Ready

volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE;

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

void delay_loop(void) {
    for (volatile int i = 0; i < 100000000; i++);
}