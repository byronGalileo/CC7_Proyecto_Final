#ifndef PCB_H
#define PCB_H

typedef struct {
    unsigned int *sp;         // Stack pointer actual del proceso
    void (*entry)(void);      // Dirección de entrada del proceso
    int pid;                  // ID (0 = OS, 1 = P1, 2 = P2)
} PCB;

#define NUM_TASKS 3

extern PCB pcbs[NUM_TASKS];
extern int current_task;
void os_init_tasks(void);


#endif
