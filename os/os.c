/* os.c */
#include "os.h"
#include "stdio.h"

PCB pcb[NUM_TASKS];
int current_task = 0;

unsigned int seed = 12345;
unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
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
    char buf[10];
    PUT32(TISR, 0x2);
    PUT32(INTC_CONTROL, 0x1);
    uart_puts("Tick\n");
    uart_puts("Tick - Task ");
    uart_itoa(current_task, buf);
    uart_puts(buf);
    uart_putc('\n');

}

void delay_loop(void) {
    for (volatile int i = 0; i < 100000000; i++);
}

void init_task_stack(PCB *task, void (*entry)(void), unsigned int *stack_top) {
    volatile unsigned int *sp = stack_top;

    // Simular el contexto que espera el irq_handler
    sp--; *sp = 0x60000010;            // CPSR
    for (int i = 0; i < 12; i++)       // R12 a R1
        sp--; *sp = 0;
    sp--; *sp = 0;                     // R0
    sp--; *sp = (unsigned int)entry;   // Fake LR ← debe ser el último valor poppeado

    task->sp = (unsigned int *)sp;
    task->stack = stack_top;
    task->state = 0;
}

void os_init_tasks(void) {
    current_task = 0;
    init_task_stack(&pcb[0], (void *)OS_ENTRY, (unsigned int *)STACK_OS_TOP);
    init_task_stack(&pcb[1], (void *)TASK1_ENTRY, (unsigned int *)STACK1_TOP);
    init_task_stack(&pcb[2], (void *)TASK2_ENTRY, (unsigned int *)STACK2_TOP);
}

void context_switch(void) {
    current_task = (current_task + 1) % NUM_TASKS;
}

int main() {
    PRINT(" \n=================== STARTING OS =================== \n");
    PRINT("Starting...\n");
    PRINT("Init tasks...\n");
    os_init_tasks();
    PRINT("Tasks initialized!\n");

    timer_init();
    enable_irq();

    PRINT("Initial TCRR: \n");
    PRINT("%x \n", GET32(TCRR));

    // Cargar SP inicial y saltar a proceso 1    
    while (1) {
        PRINT("%d \n", rand() % 1000);
        for (volatile int i = 0; i < 100000000; i++);
    }
    return 0;
}