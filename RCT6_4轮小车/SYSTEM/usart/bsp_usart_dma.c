#include "bsp_usart_dma.h"



uint8_t SendBuff[SENDBUFF_SIZE];


void USARTx_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
		// 开启DMA时钟
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		// 设置DMA源地址：串口数据寄存器地址*/
		DMA_InitStructure.DMA_PeripheralBaseAddr=USART_DR_ADDRESS;
		// 内存地址(要传输的变量的指针)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;		
		// 方向：从内存到外设	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;		
		// 传输大小	
		DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;		
		// 外设地址不增	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// 内存地址自增
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// 外设数据单位	
		DMA_InitStructure.DMA_PeripheralDataSize = 
	  DMA_PeripheralDataSize_Byte;
				// 内存数据单位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
				// DMA模式，一次或者循环模式
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
				// 优先级：中	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// 禁止内存到内存的传输
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
				// 配置DMA通道		   
		DMA_Init(DMA1_Channel4, &DMA_InitStructure);		
				// 使能DMA
		DMA_Cmd (DMA1_Channel4,ENABLE);
		
}
