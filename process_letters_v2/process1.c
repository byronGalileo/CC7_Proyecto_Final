#include "../os/os.h"
#include "../os/stdio.h"
#include "../os/pcb.h"

void main() {
    PRINT(" \n=================== STARTING LETTERS =================== \n");
    timer_init();
    enable_irq();
    os_init_tasks();
    while (1) {
        // PRINTING LETTERS FROM a TO z
        for (char c = 'a'; c <= 'z'; c++) {
            PRINT("%c \n", c);
            delay_loop();
        }
    }
}
