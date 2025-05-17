#include "os.h"
#include "stdio.h"
#include "pcb.h"
void print_pcb_info() {
    for (int i = 0; i < NUM_TASKS; i++) {
        uart_puts("[PCB] Tarea ");
        uart_putc('0' + i);
        uart_puts(" | SP: ");
        uart_puthex((unsigned int)pcbs[i].sp);
        uart_puts(" | ENTRY: ");
        uart_puthex((unsigned int)pcbs[i].entry);
        uart_putc('\n');
    }
}


int main() {
    PRINT(" \n=================== STARTING OS =================== \n");
    PRINT("Starting...\n");
    timer_init();
    enable_irq();
    os_init_tasks();

    uart_puts("[MAIN] OS iniciado. Tareas inicializadas.\n");
    print_pcb_info();
    // PRINT("Initial TCRR: \n");
    // PRINT("%x \n", GET32(TCRR));
    
    while (1) {
        PRINT("%d \n", rand() % 1000);
        for (volatile int i = 0; i < 100000000; i++);
    }
    return 0;

    
}

