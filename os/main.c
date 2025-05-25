#include "../core/os.h"
#include "../core/tasks.h"
#include "../core/timer.h"
#include "../core/sched.h"
#include "../lib/stdio.h"


int main() {
    PRINT("\n=================== STARTING OS ===================\n");
    PRINT("Starting...\n");

    os_init_tasks();
    PRINT("Tasks initialized!\n");

    timer_init();
    enable_irq();

    while (1) {
        PRINT("%d \n", rand() % 1000);
        delay_loop();
    }
    return 0;
}