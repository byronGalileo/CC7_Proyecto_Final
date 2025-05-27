#ifndef WD_H
#define WD_H

// Watchdog definitions for the AM335x platform
// The watchdog is used to reset the system if it hangs or becomes unresponsive.
// The WDT1 is used for this purpose, and it must be disabled during initialization.
// The WDT1_WSPR register is used to write to the watchdog, and WDT1_WWPS is used to check the watchdog status.
// The watchdog must be disabled before the system can run normally.

#define WDT1_BASE         0x44E35000
#define WDT1_WSPR         (*(volatile unsigned int *)(WDT1_BASE + 0x48))
#define WDT1_WWPS         (*(volatile unsigned int *)(WDT1_BASE + 0x34))

void watchdog_disable(void);

#endif 