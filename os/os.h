#ifndef OS_H
#define OS_H
#include <stdint.h>
extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);
extern void enable_irq(void);

#define UART0_BASE      0x44E09000
#define UART_THR        (UART0_BASE + 0x00)
#define UART_RHR        (UART0_BASE + 0x00)  // Receive Holding Register
#define UART_LSR        (UART0_BASE + 0x14)
#define UART_LSR_THRE   0x20
#define UART_LSR_DR  (1 << 0)  // Data Ready

#define DMTIMER2_BASE    0x48040000
#define TCLR             (DMTIMER2_BASE + 0x38)
#define TCRR             (DMTIMER2_BASE + 0x3C)
#define TISR             (DMTIMER2_BASE + 0x28)
#define TIER             (DMTIMER2_BASE + 0x2C)
#define TLDR             (DMTIMER2_BASE + 0x40)

#define INTCPS_BASE      0x48200000
#define INTC_MIR_CLEAR2  (INTCPS_BASE + 0xC8)
#define INTC_CONTROL     (INTCPS_BASE + 0x48)

#define CM_PER_BASE      0x44E00000
#define CM_PER_TIMER2_CLKCTRL (CM_PER_BASE + 0x80)

unsigned int rand(void);
void uart_putc(char c);
char uart_getc();
void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);
int uart_atoi(const char *s);
void uart_itoa(int num, char *buffer);
int32_t uart_atof(const char *str);
void uart_ftoa(float f, char *buffer, int precision);
void timer_init(void);
void timer_irq_handler(void);
void delay_loop();

#endif // OS_H
