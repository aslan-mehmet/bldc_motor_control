/* STM32F407vgt6 */

MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
	/* datasheet states 192K ram
	 * 64K core coupled memory
	 */
	ccm (rwx) : ORIGIN = 0x1000000, LENGTH = 64K
	
}

INCLUDE libopencm3_stm32f4.ld

