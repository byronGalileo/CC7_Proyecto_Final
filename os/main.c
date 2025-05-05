#include "os.h"
#include "stdio.h"

int main() {
    PRINT(" \n=================== STARTING OS =================== \n");
    PRINT("Starting...\n");
    timer_init();
    enable_irq();
    os_init_tasks();     // Inicializar PCBs
    current_task = 0;

    PRINT("Initial TCRR: \n");
    PRINT("%x \n", GET32(TCRR));

    // Cargar SP inicial y saltar a proceso 1
    asm volatile (
        "ldr sp, =0x80018000\n"   // Stack de proceso 1
        "ldr lr, =0x80010000\n"   // Código de proceso 1
        "movs pc, lr\n"
    );
    
    while (1) {
        PRINT("%d \n", rand() % 1000);
        for (volatile int i = 0; i < 100000000; i++);
    }
    return 0;
}
