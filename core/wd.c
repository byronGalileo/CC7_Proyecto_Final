#include "wd.h"

void watchdog_disable(void) {
    // Desactiva el Watchdog Timer WDT1 del AM335x
    WDT1_WSPR = 0x0000AAAA;
    while (WDT1_WWPS != 0);  // Espera a que finalice
    WDT1_WSPR = 0x00005555;
    while (WDT1_WWPS != 0);  // Espera a que finalice
}