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
    // UART base
    ldr r0, =0x44E09000

    // A: Entrando al handler
    mov r1, #'A'
    str r1, [r0]

    // Llamar al handler en C (no modificar sp allí)
    bl timer_irq_handler

    // B: Después del handler
    mov r1, #'B'
    str r1, [r0]

    // Cargar current_task
    ldr r1, =current_task
    ldr r2, [r1]         // r2 = current_task

    // C: Después de leer current_task
    mov r1, #'C'
    str r1, [r0]

    // Calcular dirección de pcbs[current_task]
    ldr r3, =pcbs
    mov r4, #12          // sizeof(PCB) = 3 * 4 bytes
    mul r2, r2, r4
    add r3, r3, r2       // r3 = &pcbs[current_task]

    // D: Después de calcular dirección del PCB
    mov r1, #'D'
    str r1, [r0]

    // Cargar SP desde PCB
    ldr r5, [r3]         // r5 = pcbs[current_task].sp
    mov sp, r5

    // E: Después de mover SP
    mov r1, #'E'
    str r1, [r0]

    // Cargar ENTRY desde PCB
    ldr r6, [r3, #4]     // r6 = pcbs[current_task].entry

    // F: Después de cargar ENTRY
    mov r1, #'F'
    str r1, [r0]

    // Mostrar dirección del salto
    mov r0, r6
    bl print_hex_uart

    // G: Después del print
    ldr r0, =0x44E09000
    mov r1, #'G'
    str r1, [r0]

    // Si es tarea 0 (OS), no saltar
    cmp r2, #0
    beq return_to_os

    // Saltar al proceso
    bx r6

return_to_os:
    mov r1, #'Z'
    str r1, [r0]
    subs pc, lr, #4

.section .bss
.align 4
_stack_bottom:
    .skip 0x2000
_stack_top:

