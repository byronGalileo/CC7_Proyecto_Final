#ifndef LIB_H
#define LIB_H

void uart_send(unsigned char x);
void uart_puts(const char *s);
void uart_putnum(unsigned int num);
void delay_loop(void);

// unsigned int GET32(unsigned int);
// void PUT32(unsigned int, unsigned int);

#endif