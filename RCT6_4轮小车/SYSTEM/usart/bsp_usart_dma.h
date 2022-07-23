#ifndef __USARTDMA_H
#define	__USARTDMA_H

#include "stm32f10x.h"
#include <stdio.h>

#define  USART_DR_ADDRESS        (USART1_BASE+0x04)
// 一次发送的数据量
#define  SENDBUFF_SIZE            500

void USARTx_DMA_Config(void);

extern uint8_t SendBuff[SENDBUFF_SIZE];

#endif /* __USARTDMA_H */
