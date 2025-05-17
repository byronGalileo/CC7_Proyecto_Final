#include "os.h"
#include "pcb.h"

PCB pcbs[NUM_TASKS];
int current_task = 0;

void os_init_tasks() {
    pcbs[0].sp = STACK_OS_TOP;
    pcbs[0].entry = OS_ENTRY;
    pcbs[0].pid = 0;

    pcbs[1].sp = STACK1_TOP;
    pcbs[1].entry = TASK1_ENTRY;
    pcbs[1].pid = 1;

    pcbs[2].sp = STACK2_TOP;
    pcbs[2].entry = TASK2_ENTRY;
    pcbs[2].pid = 2;
}
