#include "os.h"
#include "stdio.h"

void main() {
    PRINT(" \n=================== STARTING LETTERS =================== \n");
    while (1) {
        // PRINTING LETTERS FROM a TO z
        for (char c = 'a'; c <= 'z'; c++) {
            PRINT("%c \n", c);
            delay_loop();
            if (c == "k"){
                asm volatile (
                    "ldr sp, =0x80000000\n"   // Stack de proceso 1
                    "ldr lr, =0x80000000\n"   // Código de proceso 1
                    "movs pc, lr\n"
                );
            }
        }
    }
}
