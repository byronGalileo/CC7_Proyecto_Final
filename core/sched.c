#include "sched.h"
#include "tasks.h"

int current_task = 0;

void context_switch(void) {
    current_task = (current_task + 1) % NUM_TASKS;
}