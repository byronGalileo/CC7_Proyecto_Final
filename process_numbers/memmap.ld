MEMORY
{
  P2_CODE   : ORIGIN = 0x80020000, LENGTH = 32K     /* code + data + bss */
  P2_STACK  : ORIGIN = 0x80028000, LENGTH = 4K      /* stack separately */
}

SECTIONS
{
  . = ORIGIN(P2_CODE);
  .text :
  {
    *(.text*)
    *(.rodata*)
  } > P2_CODE

  .data : AT(ADDR(.text) + SIZEOF(.text))
  {
    __data_start__ = .;
    *(.data*)
    __data_end__ = .;
  } > P2_CODE

  .bss :
  {
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > P2_CODE

  _stack_bottom = ORIGIN(P2_STACK);
  _stack_top    = ORIGIN(P2_STACK) + LENGTH(P2_STACK);
}