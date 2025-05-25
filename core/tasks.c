#include "tasks.h"

PCB pcb[NUM_TASKS];

void init_task_stack(PCB *task, void (*entry)(void), unsigned int *stack_top) {
    volatile unsigned int *sp = stack_top;
    sp--; *sp = 0x60000010;
    for (int i = 0; i < 12; i++) {
    sp--;
    *sp = 0;
    }
    sp--; *sp = 0;
    sp--; *sp = (unsigned int)entry;
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