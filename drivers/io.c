#include "io.h"

void PUT32(unsigned int address, unsigned int value) {
    *((volatile unsigned int *)address) = value;
}

unsigned int GET32(unsigned int address) {
    return *((volatile unsigned int *)address);
}
