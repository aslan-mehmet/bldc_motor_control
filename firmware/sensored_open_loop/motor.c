/*
 * author: Mehmet ASLAN
 * date: October 28, 2017
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "motor.h"
#include "stm32f4xx.h"

/* 
 * l6230 enable
 * pa1 - en1
 * pa2 - en2
 * pa3 - en3
 * pa5 - diag/en -- open drain
 */
#define PORT_EN GPIOA
#define PIN_EN1 GPIO_ODR_ODR_1
#define PIN_EN2 GPIO_ODR_ODR_2
#define PIN_EN3 GPIO_ODR_ODR_3
#define PIN_DIAG GPIO_ODR_ODR_5
/*
 * timer1 pwm
 * pe9 - ch1
 * pe11 - ch2
 * pe13 - ch3
 */
#define PORT_IN GPIOE
#define PIN_IN1 GPIO_ODR_ODR_9
#define PIN_IN2 GPIO_ODR_ODR_11
#define PIN_IN3 GPIO_ODR_ODR_13

/* step switch config */
/* according motor site, technical help */
#define EN_MASK (PIN_EN1 | PIN_EN2 | PIN_EN3)
#define IN_MASK (PIN_IN1 | PIN_IN2 | PIN_IN3)

#define EN_S6 (PIN_EN2 | PIN_EN3)
#define EN_S4 (PIN_EN1 | PIN_EN3)
#define EN_S5 (PIN_EN1 | PIN_EN2)
#define EN_S1 (PIN_EN2 | PIN_EN3)
#define EN_S3 (PIN_EN1 | PIN_EN3)
#define EN_S2 (PIN_EN1 | PIN_EN2)

#define IN_S6 PIN_IN2
#define IN_S4 PIN_IN1
#define IN_S5 PIN_IN1
#define IN_S1 PIN_IN3
#define IN_S3 PIN_IN3
#define IN_S2 PIN_IN2

uint32_t en_states[] = {EN_S1, EN_S2, EN_S3, EN_S4, EN_S5, EN_S6};
uint32_t in_states[] = {IN_S1, IN_S2, IN_S3, IN_S4, IN_S5, IN_S6};

void motor_init(void)
{
	/* set up a timer(one pulse mode) and pins */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* checked init function iterates, each pin */
	GPIO_InitStructure.GPIO_Pin = PIN_EN1 | PIN_EN2 | PIN_EN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PORT_EN, &GPIO_InitStructure);
	PORT_EN->ODR &= ~EN_MASK;

	GPIO_InitStructure.GPIO_Pin = PIN_IN1 | PIN_IN2 | PIN_IN3;
	GPIO_Init(PORT_IN, &GPIO_InitStructure);
	PORT_IN->ODR &= ~IN_MASK;
	
	GPIO_InitStructure.GPIO_Pin = PIN_DIAG;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(PORT_EN, &GPIO_InitStructure);
	/* carefull because open drain */
	PORT_EN->ODR |= PIN_DIAG;
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

        /* clock_out = clock_in/(prescaler + 1)  */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        /* 1 us uev */
	TIM_TimeBaseStructure.TIM_Period = 84;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);
	/* later also setup tim1 for speed control */
}

void motor_commutate(uint8_t step_number)
{
	if (step_number > 6 || step_number == 0) {
	        return;
	}
	/* disable all logics, puts them high impedance mode */
	PORT_EN->ODR &= ~EN_MASK;
	
	/* wait to turn off */
	TIM_Cmd(TIM6, ENABLE);
	while (TIM_GetFlagStatus(TIM6, TIM_FLAG_Update) == RESET)
		;
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	
	PORT_IN->ODR &= ~IN_MASK;
        /* look hall step, set switchs accordingly */
	PORT_IN->ODR |= in_states[step_number-1];
	PORT_EN->ODR |= en_states[step_number-1];
}

void motor_set_speed(uint16_t rpm)
{}


