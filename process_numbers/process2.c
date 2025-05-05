#include "os.h"
#include "stdio.h"

void main() {
    PRINT(" \n=================== STARTING NUMBERS =================== \n");
    while (1) {
        // PRINTING NUMBERS FROM 0 TO 9
        for (int i = 0; i < 10; i++) {
            PRINT("%d \n", i);
            delay_loop();
        }
    }
}
