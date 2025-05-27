#include "../lib/stdio.h"
#include "../core/sched.h"

void data_abort_handler(void) {
    PRINT("\n[ERROR] Data Abort Exception!\n");
    while (1);
}

void svc_handler(void) {
    PRINT("\n[INFO] SVC called - switching back to OS\n");
    context_switch_and_run();
}

void print_cpu_mode(void) {
    unsigned int cpsr;
    asm volatile ("mrs %0, cpsr" : "=r" (cpsr));

    unsigned int mode = cpsr & 0x1F; 

    PRINT("[CPU MODE] \n");

    switch (mode) {
        case 0x10: PRINT("User (USR)\n"); break;
        case 0x11: PRINT("FIQ\n"); break;
        case 0x12: PRINT("IRQ\n"); break;
        case 0x13: PRINT("Supervisor (SVC)\n"); break;
        case 0x17: PRINT("Abort\n"); break;
        case 0x1B: PRINT("Undefined\n"); break;
        case 0x1F: PRINT("System\n"); break;
        default:
            PRINT("Unknown mode: 0x");
            PRINT("%d \n", mode);
    }
}