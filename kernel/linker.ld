ENTRY(kmain)
SECTIONS
{
    . = 0xffffffff80200000;

    .stivale2hdr : ALIGN(4K)
    {
        KEEP(*(.stivale2hdr))
    }

    .text : ALIGN(4K)
    {
        *(.text*)
    }

    .data : ALIGN(4K)
    {
        *(.data*)
    }

    .rodata : ALIGN(4K) 
    {
        *(.rodata*)
    }

    .bss : ALIGN(4K)
    {
        *(.bss*)
        *(COMMON)
        . = ALIGN(4K);
    }
}
