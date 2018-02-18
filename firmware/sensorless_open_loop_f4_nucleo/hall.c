/*
 * author: Mehmet ASLAN
 * date: February 3, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */

#include "stm32f4xx.h"
#include "bits.h"

void hall_sensor_init(void)
{
/**
 * hall sensor input
 * PA15 - H1 - S1
 * PB3  - H2 - S2
 * PB10 - H3 - S3
 */
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);

	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* have to implement this way */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
	EXTI_Init(&EXTI_InitStructure);

	/* so it is the most important */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

static uint8_t read_hall_sensor(void)
{
        uint16_t val = 0;

        if (GPIOB->IDR & BIT16_10) {
                val = 0x0001;
        }

        if (GPIOB->IDR & BIT16_3) {
                val |= 0x0002;
        }

        if (GPIOA->IDR & BIT16_15) {
                val |= 0x0004;
        }

        return (val & 0x07);
}

extern void motor_commutate(uint8_t step_number);
static void command_commutation(void)
{
        uint8_t val = read_hall_sensor();

        if (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET) {
		/* dont expect to motor catch, playing safe */
		USART_SendData(USART1, '0' + val);
	}

	motor_commutate(val);
}

void EXTI15_10_IRQHandler(void)
{
        if (EXTI_GetITStatus(EXTI_Line15) == SET) {
                EXTI_ClearITPendingBit(EXTI_Line15);
                command_commutation();
        }

        if (EXTI_GetITStatus(EXTI_Line10) == SET) {
                EXTI_ClearITPendingBit(EXTI_Line10);
                command_commutation();
        }
}

void EXTI3_IRQHandler(void)
{
        if (EXTI_GetITStatus(EXTI_Line3) == SET) {
                EXTI_ClearITPendingBit(EXTI_Line3);
                command_commutation();
        }
}




