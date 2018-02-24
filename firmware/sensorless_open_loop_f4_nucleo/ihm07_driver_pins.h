#ifndef __IHM07_DRIVER_PINS_H
#define __IHM07_DRIVER_PINS_H

#include "stm32f4xx.h"

/**
 * hall sensor input
 * PA15 - H1 - S1
 * PB3  - H2 - S2
 * PB10 - H3 - S3
 */
#define PORT_HALL1 GPIOA
#define PORT_HALL2_HALL3 GPIOB

#define PIN_HALL1 GPIO_Pin_15
#define PIN_HALL2 GPIO_Pin_3
#define PIN_HALL3 GPIO_Pin_10

#define PORT_HALL1_CLK RCC_AHB1Periph_GPIOA
#define PORT_HALL2_HALL3_CLK RCC_AHB1Periph_GPIOB

#define EXTI_PortSourceHALL1 EXTI_PortSourceGPIOA
#define EXTI_PortSourceHALL2_HALL3 EXTI_PortSourceGPIOB

#define EXTI_PinSourceHALL1 EXTI_PinSource15
#define EXTI_PinSourceHALL2 EXTI_PinSource3
#define EXTI_PinSourceHALL3 EXTI_PinSource10

#define EXTI_LineHALL1 EXTI_Line15
#define EXTI_LineHALL2 EXTI_Line3
#define EXTI_LineHALL3 EXTI_Line10

#define EXTI_HALL1_HALL3_IRQn EXTI15_10_IRQn
#define EXTI_HALL2_IRQn EXTI3_IRQn

#define PORT_LED_RED GPIOB
#define PIN_LED_RED GPIO_Pin_2
#define PORT_LED_RED_CLK RCC_AHB1Periph_GPIOB

#endif /* __IHM07_DRIVER_PINS_H */
