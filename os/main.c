#include "../core/os.h"
#include "../core/tasks.h"
#include "../core/timer.h"
#include "../core/sched.h"
#include "../lib/stdio.h"
#include  "../core/wd.h"

int main() {
    PRINT(" \n=================== STARTING OS =================== \n");
    PRINT("Starting...\n");
    watchdog_disable(); // Desactivar el watchdog antes de iniciar el OS
    PRINT("Init tasks...\n");

    os_init_tasks();
    dump_stack(&pcb[0], 0);
    dump_stack(&pcb[1], 1);
    PRINT("Tasks initialized!\n");

    timer_init();       // ← mover esto antes
    enable_irq();       // ← habilitar antes del primer salto

    while (1) {
        asm volatile("wfi");
    }
    return 0;
}