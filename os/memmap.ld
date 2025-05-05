MEMORY
{
    OS_CODE       : ORIGIN = 0x80000000, LENGTH = 64K
    OS_STACK      : ORIGIN = 0x80008000, LENGTH = 4K
}

SECTIONS
{
    /* OS Code and Data */
    . = ORIGIN(OS_CODE);
    .text :
    {
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
}
