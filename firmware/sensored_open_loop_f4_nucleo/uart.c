#include "stm32f4xx.h"

void uart_init(void)
{
        /**
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

	  USART_InitStructure.USART_BaudRate = 115200;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  USART_InitStructure.USART_Parity = USART_Parity_No;
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	  USART_Init(USART1, &USART_InitStructure);

	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	  /* Enable the USARTx Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	  /* Enable USART */
	  USART_Cmd(USART1, ENABLE);
}

void uart_send_byte_poll(uint8_t byt)
{
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET)
                ;
        USART_SendData(USART1, byt);
}

void uart_send_buffer_poll(uint8_t *buffer, uint8_t len)
{
        for (int i = 0; i < len; ++i) {
                uart_send_byte_poll(buffer[i]);
        }
}

