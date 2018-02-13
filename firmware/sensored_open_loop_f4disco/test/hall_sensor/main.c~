/*
 * author: Mehmet ASLAN
 * date: October 14, 2017
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "main.h"
#include "stm32f4xx_it.h"
#include "f4board.h"
/* STM32F40_41xxx */
void delay(uint32_t ms);
void tim_init(void);

int main(void)
{
	RCC_ClocksTypeDef r;
	RCC_GetClocksFreq(&r);

	/* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

	board_led_init();
	tim_init();
	
	while (1) {
		delay(100);
		GPIOD->ODR ^= LED_GREEN_ODR;
	}
}

void delay(uint32_t ms)
{
	uint64_t hold_time = get_time();
	while (get_time() - hold_time <= ms)
		;
}

void tim_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* tim2, apb1 bus = 42M */
	/* but tim2 at 84M */
	/* update event in 0.5ms */
	TIM_TimeBaseStructure.TIM_Period = 1000;
	/* clock_out = clock_in/(prescaler + 1)  */
	TIM_TimeBaseStructure.TIM_Prescaler = 83;
	/* clock div does nothing in simple timebase */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
        /* tim1, apb2 bus = 84M */
	/* but tim at 168M */
	/* update event in 0.5ms */
	TIM_TimeBaseStructure.TIM_Period = 1000;
	/* clock_out = clock_in/(prescaler + 1)  */
	TIM_TimeBaseStructure.TIM_Prescaler = 167;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	/* each (rep+1) timer ovf creates one update event */
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_TIM10_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		GPIOD->ODR ^= LED_BLUE_ODR;
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		GPIOD->ODR ^= LED_RED_ODR;
	}
}
