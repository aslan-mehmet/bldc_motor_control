#ifndef __F4NUCLEO_BOARD_H
#define __F4NUCLEO_BOARD_H

#include "stm32f4xx.h"

#define LED2 GPIO_Pin_5
#define LED2_ODR GPIO_ODR_ODR_5
#define LED2_PORT GPIOA

void board_led_init(void);

#endif /* __F4NUCLEO_BOARD_H */
