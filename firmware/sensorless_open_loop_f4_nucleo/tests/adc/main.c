/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "delay.h"
#include "uart.h"

uint8_t _adc_index = 0;
uint8_t _adc_vals[3];

void adc_init(void)
{
        /*
         * PC3 - ADC1_IN13 - BEMF1
         * PB0 - ADC1_IN8 - BEMF2
         * PA7 - ADC1_IN7 - BEMF3
         */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        ADC_InitTypeDef ADC_InitStructure;
        ADC_CommonInitTypeDef ADC_CommonInitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
        GPIO_Init(GPIOC, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
        ADC_InitStructure.ADC_ScanConvMode = ENABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 3;
        ADC_Init(ADC1, &ADC_InitStructure);

        ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
        ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
        ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
        ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
        ADC_CommonInit(&ADC_CommonInitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_28Cycles);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime_28Cycles);
        ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_28Cycles);

        ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
        ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
        ADC_Cmd(ADC1, ENABLE);
}

void ADC_IRQHandler(void)
{

        if (ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET) {
                ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
                uint8_t val = ADC_GetConversionValue(ADC1);

                if (_adc_index < 3) {
                        _adc_vals[_adc_index++] = val;
                }
        }
}

int main(void)
{
        /* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

        /* @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority */
        /*                            2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* syscfg reset, so only change interrupt you use*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	uart_init();
        adc_init();
        uint8_t arr[] = {'s', 't', 'a', 'r', 't', '\n'};
        uart_send_poll_bfr(arr, 6);

        ADC_SoftwareStartConv(ADC1);
	while (1) {
                while (_adc_index < 3)
                        ;
                uart_send_poll_bfr(_adc_vals, 3);
                _adc_index = 0;
                delay(100);
                ADC_SoftwareStartConv(ADC1);
        }
}
