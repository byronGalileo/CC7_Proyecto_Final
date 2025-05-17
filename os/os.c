/* os.c */
#include "os.h"
#include "pcb.h"

// PCB pcb[NUM_TASKS];
// int current_task = 1;

unsigned int seed = 12345;
unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

__attribute__((noinline))
void uart_putc(char c) {
    if (c == '\n') {
        while (!(GET32(UART_LSR) & UART_LSR_THRE));
        PUT32(UART_THR, '\r');
    }
    while (!(GET32(UART_LSR) & UART_LSR_THRE));
    PUT32(UART_THR, c);
}

// Function to receive a single character from UART
char uart_getc() {
    // Wait until data is available in the Receive Holding Register
    while (!(GET32(UART_LSR) & UART_LSR_DR));

    // Read character from Receive Holding Register
    return (char)GET32(UART_RHR);
}

__attribute__((noinline))
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void print_hex_uart(unsigned int val) {
    char buf[16];
    uart_puts("[IRQ] JUMP TO: ");
    uart_itoa(val, buf);  // Si querés, usa uart_puthex
    uart_puts(buf);
    uart_putc('\n');
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

void uart_puthex(unsigned int num) {
    const char *hex = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(hex[(num >> i) & 0xF]);
    }
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

void timer_init(void) {
    // Step 1: Enable timer clock
    PUT32(CM_PER_TIMER2_CLKCTRL, 0x2);

    // Step 2: Unmask IRQ 68
    PUT32(INTC_MIR_CLEAR2, 1 << (68 - 64));
    PUT32(INTCPS_BASE + 0x110, 0x0);  // INTC_ILR68: Priority 0, IRQ not FIQ

    // Step 3: Stop timer
    PUT32(TCLR, 0);

    // Step 4: Clear interrupts
    PUT32(TISR, 0x7);

    // Step 5: Set load value
    PUT32(TLDR, 0xF8D8F200);

    // Step 6: Set counter
    PUT32(TCRR, 0xF8D8F200);

    // Step 7: Enable overflow interrupt
    PUT32(TIER, 0x2);

    // Step 8: Start timer with auto-reload
    PUT32(TCLR, 0x3);

    // Timer initialized ...
}

// void timer_irq_handler(void) {
//     uart_puts("[IRQ] Tick!\n");
//     PUT32(TISR, 0x2);         // Clear timer overflow
//     PUT32(INTC_CONTROL, 0x1); // Acknowledge interrupt to interrupt controller
// }

void timer_irq_handler(void) {
    PUT32(TISR, 0x2);
    PUT32(INTC_CONTROL, 0x1);

    uart_puts("[IRQ] Tick! Switching...\n");

    // Mostrar current_task
    uart_puts("[IRQ] current_task (antes): ");
    char buf[16];
    uart_itoa(current_task, buf);
    uart_puts(buf);
    uart_putc('\n');

    // Guardar SP actual
    // asm volatile ("mov %0, sp" : "=r"(pcbs[current_task].sp));

    // Switching quemado
    if (current_task == 0)
        current_task = 1;
    else if (current_task == 1)
        current_task = 2;
    else
        current_task = 0;

    uart_puts("[IRQ] current_task (después): ");
    uart_itoa(current_task, buf);
    uart_puts(buf);
    uart_putc('\n');

    // Cargar SP del nuevo proceso
    // asm volatile ("mov sp, %0" :: "r"(pcbs[current_task].sp));
}



void delay_loop(void) {
    for (volatile int i = 0; i < 100000000; i++);
}

int schedule_state = 0;

void context_switch(void) {
    current_task = (current_task + 1) % NUM_TASKS;
}


// void os_init_tasks() {
//     // OS task (dummy context)
//     pcb[0].sp = STACK_OS_TOP - 16;
//     pcb[0].sp[15] = (unsigned int)OS_ENTRY;  // Podría ser cualquier handler del OS
//     pcb[0].sp[14] = 0x60000010;

//     pcb[1].sp = STACK1_TOP - 16;
//     pcb[1].sp[15] = (unsigned int)TASK1_ENTRY;
//     pcb[1].sp[14] = 0x60000010;

//     pcb[2].sp = STACK2_TOP - 16;
//     pcb[2].sp[15] = (unsigned int)TASK2_ENTRY;
//     pcb[2].sp[14] = 0x60000010;
// }
