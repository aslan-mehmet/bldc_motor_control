#include "motor.h"

/*
 * IN1 - UH_PWM - OUT1 - PHASE_A - PA8
 * IN2 - VH_PWM - OUT2 - PHASE_B - PA9
 * IN3 - WH_PWM - OUT3 - PHASE_C - PA10
 */
#define PORT_IN GPIOA
#define PIN_IN1 GPIO_ODR_ODR_8
#define PIN_IN2 GPIO_ODR_ODR_9
#define PIN_IN3 GPIO_ODR_ODR_10

/* for rotation direction look pins through output shaft */
/* ccw in commutation_seq.png */
#define IN_S5 PIN_IN2
#define IN_S4 PIN_IN3
#define IN_S6 PIN_IN3
#define IN_S2 PIN_IN1
#define IN_S3 PIN_IN1
#define IN_S1 PIN_IN2

uint32_t pwm_en_seq[] = {IN_S1, IN_S2, IN_S3, IN_S4, IN_S5, IN_S6};

/*
 * l6230 enable
 * Enable_CH1-L6230  - PC10
 * Enable_CH2-L6230  - PC11
 * Enable_CH3-L6230  - PC12
 * DIAG/ENABLE/BKIN1 - PA6  --> OPEN DRAIN
 */
#define PORT_EN GPIOC
#define PORT_DIAG_EN GPIOA
#define PIN_EN1 GPIO_ODR_ODR_10
#define PIN_EN2 GPIO_ODR_ODR_11
#define PIN_EN3 GPIO_ODR_ODR_12
#define PIN_DIAG GPIO_ODR_ODR_5
#define EN_MASK (PIN_EN1 | PIN_EN2 | PIN_EN3)

/* ccw in commutation_seq.png */
#define EN_S5 (PIN_EN1 | PIN_EN2)
#define EN_S4 (PIN_EN1 | PIN_EN3)
#define EN_S6 (PIN_EN2 | PIN_EN3)
#define EN_S2 (PIN_EN1 | PIN_EN2)
#define EN_S3 (PIN_EN1 | PIN_EN3)
#define EN_S1 (PIN_EN2 | PIN_EN3)

uint32_t fet_en_seq[] = {EN_S1, EN_S2, EN_S3, EN_S4, EN_S5, EN_S6};

#define TIM_PWM TIM1
#define PWM_MAX ((uint16_t) 1e3)
uint16_t pwm_val = 0;

void motor_start(void)
{
        /* Open drain mode: A “0” in the Output register activates the N-MOS whereas a “1”
         *in the Output register leaves the port in Hi-Z (the P-MOS is never activated)
         */
	/* carefull because open drain */
	PORT_DIAG_EN->ODR |= PIN_DIAG;
}

void motor_stop(void)
{
	PORT_DIAG_EN->ODR &= ~PIN_DIAG;
}

static void tim_pwm_init(void)
{
	/* clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = PIN_IN1 | PIN_IN2 | PIN_IN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PORT_IN, &GPIO_InitStructure);

	/* careful */
	GPIO_PinAFConfig(PORT_IN, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(PORT_IN, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(PORT_IN, GPIO_PinSource10, GPIO_AF_TIM1);

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

	TIM_OCInitTypeDef TIM_OCInitStructure;

	/* PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = pwm_val;
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

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

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
	GPIO_Init(PORT_DIAG_EN, &GPIO_InitStructure);
	PORT_EN->ODR &= ~PIN_DIAG;
}

void motor_set_pwm(uint16_t val)
{
	if (val > PWM_MAX) {
		val = PWM_MAX;
	}

	pwm_val = val;

	TIM_SetCompare1(TIM1, pwm_val);
	TIM_SetCompare2(TIM1, pwm_val);
	TIM_SetCompare3(TIM1, pwm_val);
}

void motor_commutate(uint8_t step_number)
{
	if (step_number > 6 || step_number == 0) {
		return;
	}

	/* float all half bridges */
	PORT_EN->ODR &= ~EN_MASK;
	/* in pins pwm duty 0 */
	TIM_PWM->CCR1 = 0;
	TIM_PWM->CCR2 = 0;
	TIM_PWM->CCR3 = 0;

	PORT_EN->ODR |= fet_en_seq[step_number-1];

	switch (pwm_en_seq[step_number-1]) {
	case PIN_IN1:
		TIM_PWM->CCR1 = pwm_val;
		break;
	case PIN_IN2:
		TIM_PWM->CCR2 = pwm_val;
		break;
	case PIN_IN3:
		TIM_PWM->CCR3 = pwm_val;
		break;
      	}
}
