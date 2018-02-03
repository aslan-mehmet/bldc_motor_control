/*
 * author: Mehmet ASLAN
 * date: October 17, 2017
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "f4board.h"
#include "delay.h"

#define PORT_IN GPIOE
#define PIN_IN1 GPIO_ODR_ODR_9
#define PIN_IN2 GPIO_ODR_ODR_11
#define PIN_IN3 GPIO_ODR_ODR_13

#define PWM_MAX 2000

void tim1_pwm_init(void)
{
	/* clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = PIN_IN1 | PIN_IN2 | PIN_IN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PORT_IN, &GPIO_InitStructure);

	/* careful */
	GPIO_PinAFConfig(PORT_IN, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(PORT_IN, GPIO_PinSource11, GPIO_AF_TIM1);
	GPIO_PinAFConfig(PORT_IN, GPIO_PinSource13, GPIO_AF_TIM1);
	
	/* timebase */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	/* 84kHz update freq */
	TIM_TimeBaseStructure.TIM_Period = PWM_MAX;
	/* clock_out = clock_in/(prescaler + 1)  */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	/* each (rep+1) timer ovf creates one update event */
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/* output compare , disable channels */
	TIM_OCInitTypeDef TIM_OCInitStructure;

	/* PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	/* ccer, cc1p active high */
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	/* enable timer & moe */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

int main(void)
{
	/* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

	board_led_init();
	tim1_pwm_init();

	uint32_t seq[] = {PIN_IN1, PIN_IN2, PIN_IN3};

	uint8_t i = 0;
	while (1) {
		delay(2);

		if (i == 3) {
			i = 0;
		}

		TIM1->CCR1 = 0;
		TIM1->CCR2 = 0;
		TIM1->CCR3 = 0;

		switch (seq[i]) {
		case PIN_IN1:
			TIM1->CCR1 = 500;
			break;
		case PIN_IN2:
			TIM1->CCR2 = 500;
			break;
		case PIN_IN3:
			TIM1->CCR3 = 500;
			break;			
		}

		PORT_LED->ODR ^= LED_RED_ODR;
		++i;
	}
}
