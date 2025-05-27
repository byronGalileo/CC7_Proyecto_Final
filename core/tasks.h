#ifndef TASKS_H
#define TASKS_H

typedef struct {
    int pid;
    unsigned int *sp;
    unsigned int *stack;
    int state;
} PCB;

#define NUM_TASKS 2
extern PCB pcb[NUM_TASKS];
extern int current_task;

#define STACK_OS_BOTTOM ((unsigned int*)0x80008000)
#define STACK1_BOTTOM   ((unsigned int*)0x80018000)
#define STACK2_BOTTOM   ((unsigned int*)0x80028000)
#define OS_ENTRY        ((void (*)(void))0x80000000)
#define TASK1_ENTRY     ((void (*)(void))0x80010000)
#define TASK2_ENTRY     ((void (*)(void))0x80020000)

void init_task_stack(PCB *task, void (*entry)(void), unsigned int *stack_bottom, int task_id);
void os_init_tasks(void);
void dump_stack(PCB *task, int task_id);

#endif