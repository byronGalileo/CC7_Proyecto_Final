#include "tasks.h"
#include "../lib/stdio.h"

PCB pcb[NUM_TASKS];

void init_task_stack(PCB *task, void (*entry)(void), unsigned int *stack_bottom, int task_id) {
    volatile unsigned int *sp = stack_bottom;

    // Stack compatible con context_switch_and_run
    sp--; *sp = 0x60000010;            // CPSR
    sp--; *sp = 0;                     // R0
    sp--; *sp = (unsigned int)entry;   // LR (entry)
    for (int i = 0; i < 12; i++) {
        sp--; *sp = 0;                 // R1-R12
    }

    task->sp = (unsigned int *)sp;
    task->stack = stack_bottom;
    task->state = 0;

    // Validación visual
    PRINT("Task %d entry: %x, SP: %x\n", task_id, (unsigned int)entry, (unsigned int)sp);
}

void os_init_tasks(void) {
    init_task_stack(&pcb[0], (void *)TASK1_ENTRY, (unsigned int *)STACK1_BOTTOM, 0); 
    init_task_stack(&pcb[1], (void *)TASK2_ENTRY, (unsigned int *)STACK2_BOTTOM, 1); 
} 

void dump_stack(PCB *task, int task_id) {
    PRINT("Dumping stack for task \n");
    PRINT("%d: \n", task_id);

    for (int i = 0; i < 16; i++) {
        unsigned int val = task->sp[i];
        PRINT("  [ %d ] = 0x%x \n", i, val); // si tenés función para imprimir en HEX
    }
} 
