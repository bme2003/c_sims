ENTRY(kernel_start)

SECTIONS
{
    . = 0x10000;
    __kernel_start = .;

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }

    __kernel_end = .;
}
