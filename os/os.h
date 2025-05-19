/*os.h*/
#ifndef OS_H
#define OS_H

extern void enable_irq(void);

#define DMTIMER2_BASE    0x48040000
#define TCLR             (DMTIMER2_BASE + 0x38)
#define TCRR             (DMTIMER2_BASE + 0x3C)
#define TISR             (DMTIMER2_BASE + 0x28)
#define TIER             (DMTIMER2_BASE + 0x2C)
#define TLDR             (DMTIMER2_BASE + 0x40)

#define INTCPS_BASE      0x48200000
#define INTC_MIR_CLEAR2  (INTCPS_BASE + 0xC8)
#define INTC_CONTROL     (INTCPS_BASE + 0x48)

#define CM_PER_BASE      0x44E00000
#define CM_PER_TIMER2_CLKCTRL (CM_PER_BASE + 0x80)

#define STACK_OS_TOP    ((unsigned int*)0x80008000)
#define STACK1_TOP      ((unsigned int*)0x80018000)
#define STACK2_TOP      ((unsigned int*)0x80028000)
#define OS_ENTRY        ((void (*)(void))0x80000000) 
#define TASK1_ENTRY     ((void (*)(void))0x80010000)
#define TASK2_ENTRY     ((void (*)(void))0x80020000)

#define NUM_TASKS 2

typedef struct {
    int pid;
    unsigned int *sp;
    unsigned int *stack;
    int state;
} PCB;

extern PCB pcb[NUM_TASKS];

unsigned int rand(void);
void timer_init(void);
void timer_irq_handler(void);
void context_switch(void);
void os_init_tasks();
void delay_loop();

#endif // OS_H
