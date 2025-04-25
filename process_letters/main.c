#include "lib.h"

int main(void) {
    uart_puts("\n[process1] Iniciando proceso...\n");
    while (1) {
        for (int i = 0; i < 10; i++) {
            uart_puts("[process1] Numero actual: ");
            uart_putnum(i);
            uart_puts("\n");
            delay_loop();
        }
    }
    return 0;
} 