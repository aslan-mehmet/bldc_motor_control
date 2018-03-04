#include "ihm07_driver.h"
#include "bits.h"

void ihm07_hall_pins_init(void)
{
        RCC_AHB1PeriphClockCmd(PORT_HALL1_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_HALL2_HALL3_CLK, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = PIN_HALL1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(PORT_HALL1, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = PIN_HALL2 | PIN_HALL3;
        GPIO_Init(PORT_HALL2_HALL3, &GPIO_InitStructure);
}

void ihm07_hall_interrupt_init(void)
{
        EXTI_InitTypeDef EXTI_InitStructure;

        SYSCFG_EXTILineConfig(EXTI_PortSourceHALL1, EXTI_PinSourceHALL1);
        SYSCFG_EXTILineConfig(EXTI_PortSourceHALL2_HALL3, EXTI_PinSourceHALL2);
        SYSCFG_EXTILineConfig(EXTI_PortSourceHALL2_HALL3, EXTI_PinSourceHALL3);

        EXTI_InitStructure.EXTI_Line = EXTI_LineHALL1;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);

        EXTI_InitStructure.EXTI_Line = EXTI_LineHALL2;
        EXTI_Init(&EXTI_InitStructure);

        EXTI_InitStructure.EXTI_Line = EXTI_LineHALL3;
        EXTI_Init(&EXTI_InitStructure);
}

void ihm07_hall_interrupt_connection_state(FunctionalState state)
{
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = EXTI_HALL1_HALL3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = state;
        NVIC_Init(&NVIC_InitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = EXTI_HALL2_IRQn;
        NVIC_Init(&NVIC_InitStructure);

}
uint8_t ihm07_hall_read(void)
{
        uint8_t hall_state = 0;

        if (PORT_HALL2_HALL3->IDR & PIN_HALL3) {
                hall_state |= BIT8_0;
        }

        if (PORT_HALL2_HALL3->IDR & PIN_HALL2) {
                hall_state |= BIT8_1;
        }

        if (PORT_HALL1->IDR & PIN_HALL1) {
                hall_state |= BIT8_2;
        }

        return hall_state;
}

void ihm07_led_red_init(void)
{
        RCC_AHB1PeriphClockCmd(PORT_LED_RED_CLK, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = PIN_LED_RED;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(PORT_LED_RED, &GPIO_InitStructure);
}

void ihm07_pwm_and_pins_init(void)
{
        RCC_AHB1PeriphClockCmd(PORT_IN1_IN2_IN3_CLK, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = PIN_IN1 | PIN_IN2 | PIN_IN3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(PORT_IN1_IN2_IN3, &GPIO_InitStructure);

        GPIO_PinAFConfig(PORT_IN1_IN2_IN3, GPIO_PinSourceIN1, GPIO_AF_TIM1);
        GPIO_PinAFConfig(PORT_IN1_IN2_IN3, GPIO_PinSourceIN2, GPIO_AF_TIM1);
        GPIO_PinAFConfig(PORT_IN1_IN2_IN3, GPIO_PinSourceIN3, GPIO_AF_TIM1);

        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        TIM_TimeBaseStructure.TIM_Prescaler = 1;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_Period = PWM_MAX_VAL;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

        TIM_OCInitTypeDef TIM_OCInitStructure;
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
        TIM_OCInitStructure.TIM_Pulse = 0;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

        TIM_OC1Init(TIM1, &TIM_OCInitStructure);
        TIM_OC2Init(TIM1, &TIM_OCInitStructure);
        TIM_OC3Init(TIM1, &TIM_OCInitStructure);

        TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
        TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
        TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

        TIM_CtrlPWMOutputs(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void ihm07_l6230_pins_init(void)
{
        RCC_AHB1PeriphClockCmd(PORT_EN1_EN2_EN3_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_GPIO_BEMF_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_DIAG_EN_CLK, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = PIN_EN1 | PIN_EN2 | PIN_EN3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(PORT_EN1_EN2_EN3, &GPIO_InitStructure);

        PORT_EN1_EN2_EN3->ODR &= (PIN_EN1 | PIN_EN2 | PIN_EN3);

        GPIO_InitStructure.GPIO_Pin = PIN_GPIO_BEMF;
        GPIO_Init(PORT_GPIO_BEMF, &GPIO_InitStructure);

        PORT_GPIO_BEMF->ODR &= ~PIN_GPIO_BEMF;

        GPIO_InitStructure.GPIO_Pin = PIN_DIAG_EN;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        GPIO_Init(PORT_DIAG_EN, &GPIO_InitStructure);
}

void ihm07_pwm_duty_interrupt_init(void)
{
        TIM_OCInitTypeDef TIM_OCInitStructure;
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Active;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
        TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
        TIM_OCInitStructure.TIM_Pulse = 1;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
        TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

        TIM_OC4Init(TIM1, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

        TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);
}


void ihm07_pwm_duty_interrupt_connection_state(FunctionalState state)
{
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);
}

void ihm07_analog_pins_init(void)
{
        RCC_AHB1PeriphClockCmd(PORT_BEMF1_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_BEMF2_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_BEMF3_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_CURR_FDBCK1_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_CURR_FDBCK2_FDBCK3_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_POT_CLK, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = PIN_BEMF1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(PORT_BEMF1, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = PIN_BEMF2;
        GPIO_Init(PORT_BEMF2, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = PIN_BEMF3;
        GPIO_Init(PORT_BEMF3, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = PIN_CURR_FDBCK1;
        GPIO_Init(PORT_CURR_FDBCK1, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = PIN_CURR_FDBCK2 | PIN_CURR_FDBCK2;
        GPIO_Init(PORT_CURR_FDBCK2_FDBCK3, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = PIN_POT;
        GPIO_Init(PORT_POT, &GPIO_InitStructure);
}

void ihm07_adc_single_mode_init(uint8_t IHM07_ADC_CH_x)
{
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

        ADC_InitTypeDef ADC_InitStructure;
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_CommonInitTypeDef ADC_CommonInitStructure;
        ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
        ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
        ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
        ADC_CommonInit(&ADC_CommonInitStructure);

        ADC_RegularChannelConfig(ADC1, IHM07_ADC_CH_x, 1, ADC_SampleTime_480Cycles);
}

uint16_t ihm07_adc_single_read_channel(uint8_t IHM07_ADC_CH_x)
{
        ADC_RegularChannelConfig(ADC1, IHM07_ADC_CH_x, 1, ADC_SampleTime_15Cycles);

        ihm07_adc_start_conversion();

        ihm07_adc_wait_conversion();

        return ihm07_adc_get_conversion_val();
}

void ihm07_adc_group_mode_init(uint8_t *IHM07_ADC_CH_x, uint8_t number_of_channels)
{
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

        ADC_InitTypeDef ADC_InitStructure;
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = ENABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = number_of_channels;
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_CommonInitTypeDef ADC_CommonInitStructure;
        ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
        ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
        ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
        ADC_CommonInit(&ADC_CommonInitStructure);

        for (int i = 0; i < number_of_channels; ++i) {
                ADC_RegularChannelConfig(ADC1, IHM07_ADC_CH_x[i], i + 1, ADC_SampleTime_480Cycles);
        }

        ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
}

void ihm07_adc_interrupt_connection_state(FunctionalState state)
{
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	NVIC_Init(&NVIC_InitStructure);
}

void ihm07_adc_dma_group_mode_init(uint8_t *IHM07_ADC_CH_x, uint32_t memory_base, uint8_t number_of_channels)
{
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

        /* pg 169 table 29 */
        /* dma2 ch 0 stream 0 */
        DMA_InitTypeDef DMA_InitStructure;
        DMA_InitStructure.DMA_Channel = DMA_Channel_0;
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
        DMA_InitStructure.DMA_Memory0BaseAddr = memory_base;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
        DMA_InitStructure.DMA_BufferSize = number_of_channels;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralBurst_Single;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
        DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
        DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
        DMA_Init(DMA2_Stream0, &DMA_InitStructure);
        DMA_Cmd(DMA2_Stream0, ENABLE);


        ADC_InitTypeDef ADC_InitStructure;
        ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
        ADC_InitStructure.ADC_ScanConvMode = ENABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = number_of_channels;
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_CommonInitTypeDef ADC_CommonInitStructure;
        ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
        ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
        ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
        ADC_CommonInit(&ADC_CommonInitStructure);

        for (int i = 0; i < number_of_channels; ++i) {
                ADC_RegularChannelConfig(ADC1, IHM07_ADC_CH_x[i], i + 1, ADC_SampleTime_3Cycles);
        }

        ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
        ADC_DMACmd(ADC1, ENABLE);
}






