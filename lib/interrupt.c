#include "interrupt.h"

extern void PUT32(unsigned int addr, unsigned int value);
extern unsigned int GET32(unsigned int addr);
extern void enable_irq(void);

PCB pcb[NUM_TASKS];
int current_task = 1;

unsigned int seed = 12345;
unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

void delay_loop(void) {
    for (volatile int i = 0; i < 100000000; i++);
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
    PUT32(TLDR, 0xFE91CA00);

    // Step 6: Set counter
    PUT32(TCRR, 0xFE91CA00);

    // Step 7: Enable overflow interrupt
    PUT32(TIER, 0x2);

    // Step 8: Start timer with auto-reload
    PUT32(TCLR, 0x3);

    // Timer initialized ...
}

void timer_irq_handler(void) {
    PUT32(TISR, 0x2);
    PUT32(INTC_CONTROL, 0x1);
    uart_puts("Tick\n");
}

int schedule_state = 0;

void context_switch(void) {
    current_task = (current_task + 1) % NUM_TASKS;
}

void os_init_tasks() {
    // OS task (dummy context)
    pcb[0].sp = STACK_OS_TOP - 16;
    pcb[0].sp[15] = (unsigned int)OS_ENTRY;  // Podría ser cualquier handler del OS
    pcb[0].sp[14] = 0x60000010;

    pcb[1].sp = STACK1_TOP - 16;
    pcb[1].sp[15] = (unsigned int)TASK1_ENTRY;
    pcb[1].sp[14] = 0x60000010;

    pcb[2].sp = STACK2_TOP - 16;
    pcb[2].sp[15] = (unsigned int)TASK2_ENTRY;
    pcb[2].sp[14] = 0x60000010;
}