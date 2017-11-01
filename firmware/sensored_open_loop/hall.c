#include "hall.h"
#include "motor.h"
#include "stm32f4xx.h"
#include "f4board.h"

void hall_sensor_init(void)
{
/* setup 3 pin change interrupts */
/* 5v tolerant */

/*
 * hall sensor input
 * pd10 - s3
 * pd11 - s2
 * pd12 - s1
 */

	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource11);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource12);
	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable and set EXTI15_10 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

}

/* isr : read pins, print to uart */
void EXTI15_10_IRQHandler(void)
{
	/* does not matter who gave the interrupt */
	EXTI_ClearITPendingBit(EXTI_Line10 | EXTI_Line11 | EXTI_Line12);

        uint16_t data = GPIO_ReadInputData(GPIOD);

	data = (data >> 10) & 0x0007;

	if (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET) {
		/* dont expect to motor catch, playing safe */
		USART_SendData(USART1, '0' + data);
	}

	motor_commutate(data);
	
	GPIOD->ODR ^= LED_RED_ODR;
}
