/*
 * author: Mehmet ASLAN
 * date: October 17, 2017
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "f4board.h"
#include "delay.h"

void hall_sensor_init(void);
void uart_init(void);

/*
 * sensor have six steps
 * cw step order : 2 3 1 5 4 6
 */


uint8_t hall_current_step = 0;

/* read motor hall sensor print to uart */
int main(void)
{
	/* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}
	
	board_led_init();
	uart_init();
	hall_sensor_init();

	uint8_t cnt = 0;
	
	while (1) {
		USART_SendData(USART1, '0' + cnt);
		if (cnt != 9) {
			++cnt;
		} else {
			cnt = 0;
		}

		delay(1000);
	}
}

void uart_init(void)
{
/* 
 * usart1
 * pb6 - tx
 * pb7 - rx
 * alternate 7
 */
	  USART_InitTypeDef USART_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  /* libc max terminal speed 38.4K baud */
	  /* no oversampling by 8, ref pg 989, err 0% */
	  USART_InitStructure.USART_BaudRate = 19200;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	  USART_Init(USART1, &USART_InitStructure);

	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	  
	  /* NVIC configuration */
	  /* Configure the Priority Group to 2 bits */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
	  /* Enable the USARTx Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
  
	  /* Enable USART */
	  USART_Cmd(USART1, ENABLE);
}

void hall_sensor_init(void)
{
/* setup 3 pin change interrupts */
/* 5v tolerant */

/*
 * hall sensor input
 * pd10
 * pd11
 * pd12 
 */

}

/* isr : read pins, print to uart */

/* 
 * uart receive interrupt 
 * needed later
 */

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		uint16_t data = USART_ReceiveData(USART1);
		if (data == 'a') {
			GPIOD->ODR ^= LED_GREEN_ODR;
		}
	}
}
