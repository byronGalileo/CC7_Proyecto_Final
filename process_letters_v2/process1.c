#include "os.h"
#include "stdio.h"

void main() {
    PRINT(" \n=================== STARTING LETTERS =================== \n");
    while (1) {
        // PRINTING LETTERS FROM a TO z
        for (char c = 'a'; c <= 'z'; c++) {
            PRINT("%c \n", c);
            delay_loop();
        }
    }
}
