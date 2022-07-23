#include "bsp_usart_dma.h"



uint8_t SendBuff[SENDBUFF_SIZE];


void USARTx_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
		// ����DMAʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		// ����DMAԴ��ַ���������ݼĴ�����ַ*/
		DMA_InitStructure.DMA_PeripheralBaseAddr=USART_DR_ADDRESS;
		// �ڴ��ַ(Ҫ����ı�����ָ��)
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;		
		// ���򣺴��ڴ浽����	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;		
		// �����С	
		DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;		
		// �����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// �ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// �������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = 
	  DMA_PeripheralDataSize_Byte;
				// �ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
				// DMAģʽ��һ�λ���ѭ��ģʽ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
				// ���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
		// ��ֹ�ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
				// ����DMAͨ��		   
		DMA_Init(DMA1_Channel4, &DMA_InitStructure);		
				// ʹ��DMA
		DMA_Cmd (DMA1_Channel4,ENABLE);
		
}
