#include <stdarg.h>
#include "string.h"

void uart_itoa(int num, char *buffer) {
    int i = 0, is_negative = 0;
    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    while (num && i < 14) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    if (is_negative) buffer[i++] = '-';
    buffer[i] = '\0';
    for (int j = 0; j < i / 2; j++) {
        char temp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }
}

void int_to_str(int num, char *buffer) {
    uart_itoa(num, buffer);
}

int uart_atoi(const char *s) {
    int num = 0, sign = 1, i = 0;
    if (s[i] == '-') {
        sign = -1;
        i++;
    }
    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num * 10 + (s[i] - '0');
    }
    return sign * num;
}

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
    return sign * (result * 1000 + (fraction * 1000 / divisor));
}

void to_string (int num, char *buffer) {
    // Convert int to string
    uart_itoa(num, buffer);
}