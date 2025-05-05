.section .text
.syntax unified
.code 32
.globl _start

_start:
    ldr sp, =_stack_top
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0
    bl main
    b .  @ Loop here instead of hang label
    b hang

hang:
    b hang

.globl PUT32
PUT32:
    str r1, [r0]
    bx lr

.globl GET32
GET32:
    ldr r0, [r0]
    bx lr

.globl enable_irq
enable_irq:
    mrs r0, cpsr
    bic r0, r0, #0x80         @ Clear I-bit
    msr cpsr_c, r0
    bx lr

.align 5
vector_table:
    b _start
    b .
    b .
    b .
    b .
    b .
    b irq_handler
    b .

irq_handler:
    /* original
    push {r0-r12, lr}
    bl timer_irq_handler
    pop {r0-r12, lr}
    subs pc, lr, #4
     */
    // Guardar contexto actual
    mrs r0, cpsr
    push {r0}
    push {r1-r12, lr}

    // Guardar SP en PCB[current_task]
    ldr r1, =pcb
    ldr r2, =current_task
    ldr r3, [r2]
    lsl r3, r3, #3          // offset = current_task * sizeof(PCB)
    add r1, r1, r3
    str sp, [r1]

    // Llamar a context_switch()
    bl context_switch

    // Cargar SP del nuevo proceso
    ldr r1, =pcb
    ldr r2, =current_task
    ldr r3, [r2]
    lsl r3, r3, #3
    add r1, r1, r3
    ldr sp, [r1]

    // Restaurar contexto
    pop {r1-r12, lr}
    pop {r0}
    msr cpsr_c, r0

    subs pc, lr, #4


.section .bss
.align 4
_stack_bottom:
    .skip 0x2000             @ 8KB stack
_stack_top:
