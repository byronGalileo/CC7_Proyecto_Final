ENTRY(_start)

SECTIONS
{
    . = 0x00010000; /* Starting address in RAM */

    .text : {
        KEEP(*(.vector_table)) /* Importante: aseguramos la vector table */
        *(.text*)
    }

    .data : AT(ADDR(.text) + SIZEOF(.text)) {
        *(.data*)
    }

    .bss : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }
}
