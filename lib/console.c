#include "console.h"
#include "../drivers/uart.h"

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