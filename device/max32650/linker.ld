#include <memory_layout.h>

MEMORY {
    BOOT (rx) : ORIGIN = BOOTLOADER_START, LENGTH = BOOTLOADER_SIZE
    APP (rx) : ORIGIN = APP_START, LENGTH = APP_SIZE
    SRAM (rwx) : ORIGIN = RAM_START, LENGTH = RAM_SIZE /* 1MB SRAM */
}

SECTIONS {
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        __bootloader_info = ALIGN(., 4);
        KEEP(*(.bootloader_info))
        KEEP(*(.flash_isr))
        *(.init*)
        *(.fini*)
        _etext = .;
    } > BOOT
    __text_len = _etext - _text;

    .bootloader_shared (NOLOAD):
    {
        KEEP(*(.bootloader_shared))
    } > SRAM

    .data :
    {
        _data = ALIGN(., 4);
        *(.text*)
        *(.rodata*)
        *(.data*)           /*read-write initialized data: initialized global variable*/
        *(.flashprog*)      /* Flash program */
        _edata = ALIGN(., 4);
    } > SRAM AT>BOOT
    __data_len = SIZEOF(.data);
    __load_data = LOADADDR(.data);

    __flash_len = __text_len + __data_len;

    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)     /*read-write zero initialized data: uninitialzed global variable*/
        *(COMMON)
        _ebss = ALIGN(., 4);
    } > SRAM
    __bss_len = SIZEOF(.bss);

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __stack_top = ORIGIN(SRAM) + LENGTH(SRAM);
    __stack_limit = __stack_top - SIZEOF(.stack_dummy);

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > SRAM

    .heap (COPY):
    {
        . = ALIGN(4);
        __heap_base = .;
        *(.heap*)
        __heap_limit = ABSOLUTE(__stack_limit);
    } > SRAM

    __HeapBase = __heap_base;
    __HeapLimit = __heap_limit;
    PROVIDE(__stack = __stack_top);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__stack_limit >= _ebss, "region RAM overflowed with stack")
}
