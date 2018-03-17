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

	  USART_InitStructure.USART_BaudRate = 1e6;
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

void uart6_stream_init(uint8_t *buf, uint8_t size)
{
        /**
         * USART6
         * PC6 TX
         * AF08
         * ref. man pg 170 table 29
         * DMA TX CH5 STREAM6
         */

        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        DMA_InitTypeDef DMA_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

        GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

        USART_InitStructure.USART_BaudRate = 1e6;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);

        DMA_InitStructure.DMA_Channel = DMA_Channel_5;
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;
        DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
        DMA_InitStructure.DMA_BufferSize = size;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
        DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
        DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
        DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
        DMA_Init(DMA2_Stream6, &DMA_InitStructure);
}

void uart6_stream_start(void)
{
        DMA_Cmd(DMA2_Stream6, ENABLE);

        while (DMA_GetCmdStatus(DMA2_Stream6) != ENABLE)
                ;

        USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);
        USART_Cmd(USART6, ENABLE);
}
