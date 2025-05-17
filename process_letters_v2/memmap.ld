MEMORY
{
  P1_CODE   : ORIGIN = 0x80010000, LENGTH = 32K
  P1_STACK  : ORIGIN = 0x80018000, LENGTH = 4K
}

SECTIONS
{
  . = ORIGIN(P1_CODE);
  
  .text :
  {
    *(.text*)
    *(.rodata*)
  } > P1_CODE

  .data : AT(ADDR(.text) + SIZEOF(.text))
  {
    __data_start__ = .;
    *(.data*)
    __data_end__ = .;
  } > P1_CODE

  .bss :
  {
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > P1_CODE

  /* Stack starts at 0x80018000 — defined manually */
  _stack_bottom = ORIGIN(P1_STACK);
  _stack_top    = ORIGIN(P1_STACK) + LENGTH(P1_STACK); 
}
