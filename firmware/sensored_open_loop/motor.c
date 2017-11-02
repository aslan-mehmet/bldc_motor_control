#include "motor.h"

#define PORT_IN GPIOE
#define PIN_IN1 GPIO_ODR_ODR_9
#define PIN_IN2 GPIO_ODR_ODR_11
#define PIN_IN3 GPIO_ODR_ODR_13

#define TIM_PWM TIM1
#define TIM_CH1 TIM_CCER_CC1E
#define TIM_CH2 TIM_CCER_CC2E
#define TIM_CH3 TIM_CCER_CC3E
#define TIM_CH_MASK (TIM_CH1 | TIM_CH2 | TIM_CH3)

uint8_t motor_state = ENABLE;
/* cw 
 * pha out1
 * phb out2
 * phc out3
 */
#define IN_S5 TIM_CH2
#define IN_S4 TIM_CH3
#define IN_S6 TIM_CH3
#define IN_S2 TIM_CH1
#define IN_S3 TIM_CH1
#define IN_S1 TIM_CH2

uint32_t pwm_en_seq[] = {IN_S1, IN_S2, IN_S3, IN_S4, IN_S5, IN_S6};

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
#define EN_MASK (PIN_EN1 | PIN_EN2 | PIN_EN3)

#define EN_S5 (PIN_EN1 | PIN_EN2)
#define EN_S4 (PIN_EN1 | PIN_EN3)
#define EN_S6 (PIN_EN2 | PIN_EN3)
#define EN_S2 (PIN_EN1 | PIN_EN2)
#define EN_S3 (PIN_EN1 | PIN_EN3)
#define EN_S1 (PIN_EN2 | PIN_EN3)

uint32_t fet_en_seq[] = {EN_S1, EN_S2, EN_S3, EN_S4, EN_S5, EN_S6};

void motor_start(void)
{
	motor_state = ENABLE;	
}

void motor_stop(void)
{
	motor_state = DISABLE;
	TIM_PWM->CCER &= ~TIM_CH_MASK;
}

void tim_pwm_init(void)
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
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 1;
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

void motor_init(void)
{
	tim_pwm_init();

		/* set up a timer(one pulse mode) and pins */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
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
	
	GPIO_InitStructure.GPIO_Pin = PIN_DIAG;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(PORT_EN, &GPIO_InitStructure);
	/* carefull because open drain */
	PORT_EN->ODR |= PIN_DIAG;
}

void motor_set_pwm(uint16_t val)
{
	if (val >= PWM_MAX) {
		val = PWM_MAX -1;
	} else if (val == 0){
		val = 1;
	}
	
	TIM_SetCompare1(TIM1, val);
	TIM_SetCompare2(TIM1, val);
	TIM_SetCompare3(TIM1, val);
}

void motor_commutate(uint8_t step_number)
{
	if (motor_state == DISABLE) {
		return;
	}
	
	PORT_EN->ODR &= ~EN_MASK;
	TIM_PWM->CCER &= ~TIM_CH_MASK;

	PORT_EN->ODR |= fet_en_seq[step_number-1];
        TIM_PWM->CCER |= pwm_en_seq[step_number-1];
}
