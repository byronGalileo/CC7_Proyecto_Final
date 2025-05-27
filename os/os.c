/* os.c */
#include "os.h"
#include "stdio.h"

PCB pcb[NUM_TASKS];

unsigned int seed = 12345;
unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}