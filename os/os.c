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
}

void delay_loop(void) {
    for (volatile int i = 0; i < 100000000; i++);
}

void init_task_stack(PCB *task, void (*entry)(void), unsigned int *stack_top, int task_id) {
    volatile unsigned int *sp = stack_top;

    // Stack compatible con context_switch_and_run
    sp--; *sp = 0x60000010;            // CPSR
    sp--; *sp = 0;                     // R0
    sp--; *sp = (unsigned int)entry;   // LR (entry)
    for (int i = 0; i < 12; i++)
        sp--; *sp = 0;                 // R1-R12

    task->sp = (unsigned int *)sp;
    task->stack = stack_top;
    task->state = 0;

    // Validación visual
    PRINT("Task %d entry: %x, SP: %x\n", task_id, (unsigned int)entry, (unsigned int)sp);
}

void os_init_tasks(void) {
    init_task_stack(&pcb[0], (void *)TASK1_ENTRY, (unsigned int *)STACK1_TOP, 0);
    init_task_stack(&pcb[1], (void *)TASK2_ENTRY, (unsigned int *)STACK2_TOP, 1);
}

void context_switch(void) {
    current_task = (current_task) % NUM_TASKS;
}

void context_switch_and_run(void) {
    unsigned int *next_sp = pcb[current_task].sp;

    asm volatile (
        "mov sp, %[sp]\n"
        "pop {r1-r12, lr}\n"
        "pop {r0}\n"
        "msr cpsr_c, r0\n"
        "movs pc, lr\n"  // ← en vez de subs pc, lr, #4
        :
        : [sp] "r"(next_sp)
    );
}

void data_abort_handler(void) {
    PRINT("\n[ERROR] Data Abort Exception!\n");
    while (1);
}

void svc_handler(void) {
    PRINT("\n[INFO] SVC called - switching back to OS\n");
    context_switch_and_run();
}

void dump_stack(PCB *task, int task_id) {
    uart_puts("Dumping stack for task ");
    char idbuf[4];
    uart_itoa(task_id, idbuf);
    uart_puts(idbuf);
    uart_puts(":\n");

    for (int i = 0; i < 16; i++) {
        unsigned int val = task->sp[i];
        char buf[12];
        uart_itoa(val, buf);
        uart_puts("  [");
        char index_buf[4];
        uart_itoa(i, index_buf);
        uart_puts(index_buf);
        uart_puts("] = 0x");
        PRINT(" %x \n", val);  // si tenés función para imprimir en HEX
    }
}

int main() {
    PRINT(" \n=================== STARTING OS =================== \n");
    PRINT("Starting...\n");
    PRINT("Init tasks...\n");
    os_init_tasks();
    dump_stack(&pcb[0], 0);
    dump_stack(&pcb[1], 1);
    PRINT("Tasks initialized!\n");

    timer_init();
    enable_irq();

    PRINT("Initial TCRR: \n");
    PRINT("%x \n", GET32(TCRR)); 

    // Cargar SP inicial y saltar a proceso 1    
    while (1) {
        //PRINT("%d \n", rand() % 1000);
        for (volatile int i = 0; i < 100000000; i++);
    }
    return 0;
}