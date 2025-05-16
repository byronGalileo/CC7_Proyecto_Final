#ifndef PCB_H
#define PCB_H

typedef struct {
    const char *name;
    void (*entry_point)(void);
    unsigned int *stack_ptr;
    unsigned int pid;
} PCB;

// Podés declarar una tabla de procesos aquí si querés inicializarla en un .c
extern PCB process_table[];
extern const int process_count;

#endif // PCB_H
