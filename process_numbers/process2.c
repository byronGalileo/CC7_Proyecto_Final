#include "os.h"
#include "stdio.h"

void main() {
    PRINT(" \n=================== STARTING NUMBERS =================== \n");
    while (1) {
        // PRINTING NUMBERS FROM 0 TO 9
        for (int i = 0; i < 10; i++) {
            PRINT("%d \n", i);
            delay_loop();
            if (i == 9){
                asm volatile (
                    "ldr sp, =0x80000000\n"   // Stack de proceso 1
                    "ldr lr, =0x80000000\n"   // Código de proceso 1
                    "movs pc, lr\n"
                );
            }
        }
    }
}
