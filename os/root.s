.section .text
.syntax unified
.code 32
.globl _start

.extern _os_stack_top
.extern _os_stack_bottom

.globl STACK_OS_TOP
.set STACK_OS_TOP, _os_stack_top

.globl STACK_OS_BOTTOM
.set STACK_OS_BOTTOM, _os_stack_bottom

_start:
    ldr sp, =_os_stack_top
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
    b svc_handler        @ SVC (Software Interrupt)
    b .
    b data_abort_handler @ Data Abort
    b .
    b irq_handler
    b .

.globl irq_handler
irq_handler:
    ldr sp, =STACK_OS_TOP        @ usar stack seguro del OS

    mrs r0, cpsr
    push {r0}
    push {r1-r12, lr}

    // Guardar SP en pcb[current_task].sp
    ldr r1, =pcb
    ldr r2, =current_task
    ldr r3, [r2]
    lsl r3, r3, #4               @ sizeof(PCB) = 16 bytes
    add r1, r1, r3
    str sp, [r1]                 @ pcb[current_task].sp = sp

    bl timer_irq_handler        @ solo marca tick_flag

    ldr sp, =STACK_OS_TOP       @ restaurar stack OS
    bl context_switch_and_run

    b hang                      @ nunca debería llegar aquí


svc_handler:
    ldr sp, =STACK_OS_TOP       @ restaurar stack seguro del OS
    bl context_switch_and_run
    subs pc, lr, #4             @ regresar al punto de interrupción

data_abort_handler:
    ldr sp, =STACK_OS_TOP       @ restaurar stack seguro del OS
    bl uart_puts
    .asciz "\n[ERROR] Data Abort Exception!\n"
.loop_abort:
    b .loop_abort               @ ciclo infinito

.section .bss
.align 4
_stack_bottom:
    .skip 0x2000             @ 8KB stack
_stack_top:
