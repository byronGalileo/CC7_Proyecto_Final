#include "sched.h"
#include "tasks.h"
#include "../lib/stdio.h"

int current_task = 0;

void context_switch(void) {
    current_task = (current_task + 1) % NUM_TASKS;

    PRINT("[context_switch] New task: %d\n", current_task); 
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