/*
 * Author: Mehmet ASLAN
 * Date: October 14, 2017
 */
ENTRY(Reset_Handler)

/* Define sections. */
SECTIONS
{
	.isr_vector : {
	/* ffunction-sections optimization opts out interrupt functions */
	/* that is why keep needed */
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} >rom

	.text : {
		*(.vectors)	/* Vector table */
		*(.text*)	/* Program code */
		. = ALIGN(4);
		*(.rodata*)	/* Read-only data */
		. = ALIGN(4);
	} >rom

	. = ALIGN(4);
	_etext = .;

	.data : {
		_sdata = .;
		*(.data*)	/* Read-write initialized data */
		. = ALIGN(4);
		_edata = .;
	} >ram AT >rom
	/* start of initialized data vals(load memory address) */
	_sidata = LOADADDR(.data);

	.bss : {
	/* /usr/bin/../lib/gcc/arm-none-eabi/6.3.1/../../../../arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard/crt0.o: In function `_start': */
	/* above needs __bss_start__ */
		__bss_start__ = .;
	        _sbss = .;
		*(.bss*)	/* Read-write zero initialized data */
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = .;
	} >ram


	. = ALIGN(4);
	end = .;
}

PROVIDE(_estack = ORIGIN(ram) + LENGTH(ram));

