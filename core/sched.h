#ifndef SCHED_H
#define SCHED_H

extern int current_task;
void context_switch(void);
void context_switch_and_run(void);

#endif
