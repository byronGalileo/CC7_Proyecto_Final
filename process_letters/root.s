.section .vectors, "ax"
.syntax unified
.code 32
.globl _start

_start:
    ldr sp, =_stack_top @ Initialize stack pointer
    bl main @ Call main
    b hang @ Infinite loop

hang:
    b hang

.section .bss
.align 4
_stack_bottom:
    .skip 0x1000 @ Reserve 4KB for stack
_stack_top:
