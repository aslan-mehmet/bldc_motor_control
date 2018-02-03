#ifndef __F4BOARD_H
#define __F4BOARD_H

/* leds on portd */
#define LED_GREEN GPIO_Pin_12
#define LED_ORANGE GPIO_Pin_13
#define LED_RED GPIO_Pin_14
#define LED_BLUE GPIO_Pin_15

#define LED_GREEN_ODR GPIO_ODR_ODR_12
#define LED_ORANGE_ODR GPIO_ODR_ODR_13
#define LED_RED_ODR GPIO_ODR_ODR_14
#define LED_BLUE_ODR GPIO_ODR_ODR_15
#define LED_ODR_MASK (LED_GREEN_ODR | LED_ORANGE_ODR | LED_RED_ODR | LED_BLUE_ODR)

#define PORT_LED GPIOD

void board_led_init(void);

#endif	/* __F4BOARD_H */
