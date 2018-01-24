#include <stdint.h>
/**
 * @file syscheck.c
 * @author Mehmet ASLAN
 * @date November 19, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief
 * Check your system so variable representations same with microcontroller\n
 * WHY BOTHER THIS MUCH?\n
 * whole purpose of this lib directly sending what is on the memory of the microcontroller\n
 * without additional logic 
 */
int8_t sys_check(void)
{
	/* check */
	/* size */
	uint32_t u32;
	float f;
	int32_t i32;
	uint8_t *p = (uint8_t *) &u32;
	
        if (sizeof(u32) != 4) { /* something really wrong */
		return -1;
	}

	if (sizeof(f) != 4) {
		return -2;
	}


	/* 2's complement */
       	u32 = 0xffffff9c; 

	i32 = -100;
	uint8_t *t = (uint8_t *) &i32;
	for (int i = 0; i < 4; ++i) {
		if (p[i] != t[i]) {
			return -3;
		}
	}
	
	/* little endian */
	u32 = 0x12345678;
	uint8_t little[4] = {0x78, 0x56, 0x34, 0x12};
	for (int i = 0; i < 4; ++i) {
		if (little[i] != p[i]) {
			return -4;
		}
	}
	
	/* decimal number representations */
	f = -248.75;
	u32 = 0xc378c000;	/* ieee 754 std */
	uint8_t *fp = (uint8_t *) &f;

	for (int i = 0; i < 4; ++i) {
		if (fp[i] != p[i]) {
			return -5;
		}
	}

	/* also memory allows byte access */
	return 0;
}
