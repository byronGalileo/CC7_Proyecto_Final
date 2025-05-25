MEMORY
{
    OS_CODE       : ORIGIN = 0x80000000, LENGTH = 64K
    IRQ_STACK     : ORIGIN = 0x80007000, LENGTH = 4K
    OS_STACK      : ORIGIN = 0x80008000, LENGTH = 4K
}

SECTIONS
{
    /* OS Code and Data */
    . = ORIGIN(OS_CODE);
    .text :
    {
        KEEP(*(.vectors))     /* ← Aquí caerá _start desde root.s */
        *(.text*)
        *(.rodata*)
    } > OS_CODE

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > OS_CODE

    .bss :
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > OS_CODE

    /* Define OS Stack Top and Bottom */
    _os_stack_bottom = ORIGIN(OS_STACK);
    _os_stack_top    = ORIGIN(OS_STACK) + LENGTH(OS_STACK);

    /* Stack modo IRQ */
    _irq_stack_bottom = ORIGIN(IRQ_STACK);
    _irq_stack_top    = ORIGIN(IRQ_STACK) + LENGTH(IRQ_STACK);
}
