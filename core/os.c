#include "os.h"

unsigned int seed = 12345;
unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

void delay_loop(void) {
    for (volatile int i = 0; i < 100000000; i++);
}