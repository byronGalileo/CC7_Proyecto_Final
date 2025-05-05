#include "lib.h"

#define UART0_BASE 0x101f1000

#define UART_DR      0x00  // Data Register
#define UART_FR      0x18  // Flag Register
#define UART_FR_TXFF 0x20  // Transmit FIFO Full
#define UART_FR_RXFE 0x10  // Receive FIFO Empty

volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE;

// Function to send a single character via UART
void uart_putc(char c) {
    // Wait until there is space in the FIFO
    while (UART0[UART_FR / 4] & UART_FR_TXFF);
    UART0[UART_DR / 4] = c;
}

void uart_putnum(unsigned int num) {
    char buf[10];
    int i = 0;
    do {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);
    while (i > 0) {
        uart_putc(buf[--i]);
    }
}

void delay_loop(void) {
    for (volatile int i = 0; i < 500000000; i++);
}

// Function to receive a single character via UART
char uart_getc() {
    // Wait until data is available
    while (UART0[UART_FR / 4] & UART_FR_RXFE);
    return (char)(UART0[UART_DR / 4] & 0xFF);
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
    while (i < max_length - 1) { // Leave space for null terminator
        c = uart_getc();
        if (c == '\n' || c == '\r') {
            uart_putc('\n'); // Echo newline
            break;
        }

        if (c == 0x08 || c == 0x7F) { // Backspace o DEL
            if (i > 0) {
                uart_putc('\b');  // Mueve el cursor atrás
                uart_putc(' ');    // Borra el carácter en pantalla
                uart_putc('\b');  // Mueve el cursor atrás nuevamente
                i--;
            }
        } else {
            uart_putc(c); // Eco del carácter ingresado
            buffer[i++] = c;
        }
    }
    buffer[i] = '\0'; // Null terminate the string
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
float uart_atof(const char *s) {
    float result = 0.0, fraction = 0.0, divisor = 10.0;
    int sign = 1, i = 0, decimal_found = 0;

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
                fraction += (s[i] - '0') / divisor;
                divisor *= 10.0;
            } else {
                result = result * 10.0 + (s[i] - '0');
            }
        }
        i++;
    }

    return sign * (result + fraction);
}

// Function to print a float
void uart_ftoa(float num, char *buffer, int precision) {
    int integerPart = (int)num;
    float fractionalPart = num - integerPart;

    // Convert integer part to string
    uart_itoa(integerPart, buffer);

    // Find the end of the integer part string
    int i = 0;
    while (buffer[i] != '\0') {
        i++;
    }

    // Add decimal point
    buffer[i++] = '.';

    // Convert fractional part to string
    for (int j = 0; j < precision; j++) {
        fractionalPart *= 10;
        int digit = (int)fractionalPart;
        buffer[i++] = '0' + digit;
        fractionalPart -= digit;
    }

    // Null-terminate the string
    buffer[i] = '\0';
}