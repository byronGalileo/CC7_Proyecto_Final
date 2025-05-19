@ file: root.s
@ brief: Root assembly file for the OS
.section .text
.syntax unified
.code 32
.globl _start

.extern _os_stack_top
.extern _os_stack_bottom
.extern pcb
.extern current_task
.extern context_switch
.extern context_switch_and_run
.extern timer_irq_handler
.extern uart_puts

.globl STACK_OS_TOP
.set STACK_OS_TOP, _os_stack_top

.globl STACK_OS_BOTTOM
.set STACK_OS_BOTTOM, _os_stack_bottom

_start:
    ldr sp, =_os_stack_top
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
    b svc_handler        @ SVC (Software Interrupt)
    b .
    b data_abort_handler @ Data Abort
    b .
    b irq_handler
    b .

.globl irq_handler
irq_handler:
    // Guardar SP del proceso actual en r4
    mov r4, sp

    // Guardar CPSR y registros del proceso
    mrs r0, cpsr
    push {r0}
    push {r1-r3, r5-r12, lr}     // r4 se omite a propósito

    // Guardar SP en pcb[current_task].sp
    ldr r0, =current_task
    ldr r1, [r0]
    ldr r2, =pcb
    lsl r1, r1, #4
    add r2, r2, r1
    str r4, [r2]                 // pcb[current_task].sp = sp

    // Cambiar a stack seguro del OS
    ldr sp, =STACK_OS_TOP

    // Lógica del OS
    bl timer_irq_handler
    bl context_switch

    // Saltar al nuevo proceso
    bl context_switch_and_run

svc_handler:
    ldr sp, =STACK_OS_TOP       @ restaurar stack seguro del OS
    bl context_switch_and_run
    movs pc, lr                 @ regresar al punto de interrupción

data_abort_handler:
    ldr sp, =STACK_OS_TOP       @ restaurar stack seguro del OS
    ldr r0, =abort_msg
    bl uart_puts
.loop_abort:
    b .loop_abort

.section .rodata
abort_msg:
    .asciz "\n[ERROR] Data Abort Exception!\n"

.section .bss
.align 4
_stack_bottom:
    .skip 0x2000             @ 8KB stack
_stack_top:
