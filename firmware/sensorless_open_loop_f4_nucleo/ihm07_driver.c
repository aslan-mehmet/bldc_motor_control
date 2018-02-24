#include "ihm07_driver.h"
#include "bits.h"

void ihm07_hall_pins_init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        RCC_AHB1PeriphClockCmd(PORT_HALL1_CLK, ENABLE);
        RCC_AHB1PeriphClockCmd(PORT_HALL2_HALL3_CLK, ENABLE);

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
