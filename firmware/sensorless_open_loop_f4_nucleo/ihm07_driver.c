#include "ihm07_driver.h"
#include "bits.h"


void ihm07_hall_pins_init(void)
{
        /**
         * hall sensor input
         * PA15 - H1 - S1
         * PB3  - H2 - S2
         * PB10 - H3 - S3
         */
	GPIO_InitTypeDef   GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ihm07_hall_state_change_callback(void) __attribute__ ((weak));

void ihm07_hall_interrupt_init(void)
{
        EXTI_InitTypeDef   EXTI_InitStructure;

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
}

void ihm07_hall_interrupt_set_state(FunctionalState state)
{
        NVIC_InitTypeDef   NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

uint8_t ihm07_hall_read(void)
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

void ihm07_board_and_pins_init(void)
{
        RCC_AHB1PeriphClockCmd(PORT_DIAG_EN_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(PORT_GPIO_BEMF_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = PIN_DIAG_EN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PORT_DIAG_EN, &GPIO_InitStructure);

	        /* float all the channels */
	PORT_DIAG_EN->ODR &= ~PIN_DIAG_EN_ODR;

	GPIO_InitStructure.GPIO_Pin = PIN_GPIO_BEMF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(PORT_GPIO_BEMF, &GPIO_InitStructure);

	PORT_GPIO_BEMF &= ~PIN_GPIO_BEMF_ODR;
}

void ihm07_pwm_and_pins_init(void)
{
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* checked init function iterates, each pin */
	GPIO_InitStructure.GPIO_Pin = PIN_EN1 | PIN_EN2 | PIN_EN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(PORT_EN, &GPIO_InitStructure);
        /* float each channel */
	PORT_EN->ODR &= ~(PIN_EN1_ODR | PIN_EN2_ODR | PIN_EN3_ODR); 

	
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

void ihm07_led_red_init(void)
{
        /* PB2 - LED_RED */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ihm07_adc_init(void)
{
        /*
         * PC3 - ADC1_IN13 - BEMF1
         * PB0 - ADC1_IN8 - BEMF2
         * PA7 - ADC1_IN7 - BEMF3
         * PB1 - ADC1_IN9 - POT
         */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        ADC_InitTypeDef ADC_InitStructure;
        ADC_CommonInitTypeDef ADC_CommonInitStructure;

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
        ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
        ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
        ADC_CommonInit(&ADC_CommonInitStructure);

        ADC_Cmd(ADC1, ENABLE);
}

uint16_t ihm07_adc_read_single_channel(uint8_t channel)
{
        ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_28Cycles);

        ADC_SoftwareStartConv(ADC1);

        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
                ;

        return ADC_GetConversionValue(ADC1);
}



