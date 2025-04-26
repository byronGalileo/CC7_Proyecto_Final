#include "os.h"
#include "stdio.h"

int main() {
    PRINT(" \n=================== STARTING OS =================== \n");
    PRINT("Starting...\n");
    timer_init();
    enable_irq();

    PRINT("Initial TCRR: \n");
    PRINT("%x \n", GET32(TCRR));

    while (1) {
        PRINT("%d \n", rand() % 1000);
        for (volatile int i = 0; i < 100000000; i++);
    }
    return 0;
}
