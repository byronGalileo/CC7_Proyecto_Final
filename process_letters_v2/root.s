.section .text
.syntax unified
.code 32
.globl _start

_start:
    ldr sp, =_stack_top         @ Initialize stack pointer
    ldr r0, =vector_table       @ Load address of vector table
    mcr p15, 0, r0, c12, c0, 0  @ Set vector base address (VBAR)
    bl main                     @ Call main
    b hang                      @ Infinite loop if main returns

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
    b _start         @ Reset
    b .              @ Undefined Instruction
    b .              @ SWI
    b .              @ Prefetch Abort
    b .              @ Data Abort
    b .              @ Not used
    b irq_handler    @ IRQ
    b .              @ FIQ

irq_handler:
    ldr r0, =0x44E09000
    mov r1, #'I'
    str r1, [r0]

    bl timer_irq_handler

    ldr r3, =pcbs
    ldr r5, [r3]         @ Load sp from pcbs[0]
    mov sp, r5

    ldr r6, [r3, #4]     @ Load entry from pcbs[0]
    bx r6



.section .bss
.align 4
_stack_bottom:
    .skip 0x1000 @ Reserve 4KB for stack
_stack_top:

