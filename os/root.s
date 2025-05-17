.section .text
.syntax unified
.code 32
.globl _start

_start:
    // Configurar stack para modo Supervisor (main)
    ldr sp, =_os_stack_top

    // Cambiar a modo IRQ para configurar su stack
    cpsid i                @ Desactiva interrupciones
    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0x12      @ IRQ mode
    msr cpsr_c, r0
    ldr sp, =_irq_stack_top

    // Volver a modo Supervisor
    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0x13      @ Supervisor mode
    msr cpsr_c, r0
    cpsie i                @ Rehabilita interrupciones

    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0
    bl main
    b .

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
    push {r0-r12, lr}        @ Guarda registros generales
    bl timer_irq_handler     @ Llama al manejador C que hace PUT32 para limpiar el timer y escribe "Tick"
    pop {r0-r12, lr}         @ Restaura los registros
    subs pc, lr, #4          @ Retorna de la interrupción


.section .bss
.align 4
_stack_bottom:
    .skip 0x2000
_stack_top:

