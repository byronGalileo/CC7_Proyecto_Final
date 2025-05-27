#include "timer.h"
#include "../drivers/io.h"
#include "../drivers/uart.h"
#include "../core/tasks.h"
#include "../lib/string.h"

void timer_init(void) {
    PUT32(CM_PER_TIMER2_CLKCTRL, 0x2);
    PUT32(INTC_MIR_CLEAR2, 1 << (68 - 64));
    PUT32(INTCPS_BASE + 0x110, 0x0);
    PUT32(TCLR, 0);
    PUT32(TISR, 0x7);
    PUT32(TLDR, 0xFE91CA00);
    PUT32(TCRR, 0xFE91CA00);
    PUT32(TIER, 0x2);
    PUT32(TCLR, 0x3);
}

void timer_irq_handler(void) {
    PUT32(TISR, 0x2);
    PUT32(INTC_CONTROL, 0x1);
    // PRINT("Tick\n");
    char buf[10];
    uart_puts("[IRQ] Switching from task ");
    uart_itoa(current_task, buf);
    uart_puts(buf);
    uart_puts("\n");
}