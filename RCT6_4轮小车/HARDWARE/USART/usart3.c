/* SYSTEM */
#include "delay.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "string.h"
/* HARDWARE */
#include "usart3.h"
#include "usart.h"
#include "led.h"
#include "oled.h"

FIFO2_DMA_H Usart3_Handle;

void USART3_IRQHandler(void)
{
    u8 *p;
    u8 USART3_RX_LEN = 0;                                  //�������ݳ���
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //����3�����ж�
    {
        USART_ReceiveData(USART3);                                  //�������2�����ж�IDLE��־λ
        USART_ClearFlag(USART3, USART_FLAG_TC);                     //���USART2��־λ
        DMA_Cmd(DMA1_Channel3, DISABLE);                            //�ر�USART2 TX DMA1 ��ָʾ��ͨ��
        USART3_RX_LEN = USART3_MAX_RECV_LEN - DMA1_Channel3->CNDTR; //��ý��յ����ֽ���
        if (Usart3_Handle.witchbuf)                                 //֮ǰ�õ�u2rxbuf���л�Ϊu1rxbuf
        {
            p = Usart3_Handle.USARTx_RX_BUF2;                        //�ȱ���ǰһ�����ݵ�ַ���л�������
            DMA1_Channel3->CMAR = (u32)Usart3_Handle.USARTx_RX_BUF1; //�л�Ϊu1rxbuf��������ַ
            Usart3_Handle.witchbuf = 0;                              //��һ���л�Ϊu2rxbuf
        }
        else //֮ǰ�õ�u1rxbuf���л�Ϊu2rxbuf
        {
            p = Usart3_Handle.USARTx_RX_BUF1;                        //�ȱ���ǰһ�����ݵ�ַ���л�������
            DMA1_Channel3->CMAR = (u32)Usart3_Handle.USARTx_RX_BUF2; //�л�Ϊu2rxbuf��������ַ
            Usart3_Handle.witchbuf = 1;                              //��һ���л�Ϊu1rxbuf
        }
        DMA1_Channel3->CNDTR = USART3_MAX_RECV_LEN; // DMAͨ����DMA����Ĵ�С
        DMA_Cmd(DMA1_Channel3, ENABLE);             //ʹ��USART2 TX DMA1 ��ָʾ��ͨ��

        //******************�������������������ݴ������������******************//

        DMA_USART3_Tx_Data(p, USART3_RX_LEN);

        //******************�������������������ݴ������������******************//
    }
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}

//��ʼ��IO ����3
// pclk1:PCLK1ʱ��Ƶ��(Mhz)
// bound:������
void usart3_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); // GPIOCʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); // GPIOCʱ��

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //����3ʱ��ʹ��
    //������ӳ��
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);

    USART_DeInit(USART3); //��λ����
    // USART3_TX   PC10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);          //��ʼ��PB10

    // USART3_RX	  PC11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
    GPIO_Init(GPIOC, &GPIO_InitStructure);                //��ʼ��PB11

    USART_InitStructure.USART_BaudRate = bound;                                     //������һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure); //��ʼ������	3

    USART_Cmd(USART3, ENABLE); //ʹ�ܴ���

    //ʹ�ܽ����ж�
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //�����ж�
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); //������⴮�ڿ���״̬�ж�
    USART_ClearFlag(USART3, USART_FLAG_TC);
    //�����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14; //��ռ���ȼ�7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��VIC�Ĵ���

    Usart3_DMA_Init();
}

void Usart3_DMA_Init(void)
{
    DMA_InitTypeDef DMA1_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // DMA_USART3_RX  USART3->RAM�����ݴ���
    DMA_DeInit(DMA1_Channel3);                                                 //��DMA��ͨ��6�Ĵ�������Ϊȱʡֵ
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);            //��������ǰװ��ʵ��RAM��ַ
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart3_Handle.USARTx_RX_BUF1; //���ý��ջ������׵�ַ
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                        //���ݴ��䷽�򣬴������ȡ���ڴ�
    DMA1_InitStructure.DMA_BufferSize = USART3_MAX_RECV_LEN;                   // DMAͨ����DMA����Ĵ�С
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //�����ַ�Ĵ�������
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //�ڴ��ַ�Ĵ�������
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                             //����������ģʽ
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                       // DMAͨ�� xӵ�и����ȼ�
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                              // DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel3, &DMA1_InitStructure);

    // DMA_USART3_TX  RAM->USART3�����ݴ���
    DMA_DeInit(DMA1_Channel2);                                                //��DMA��ͨ��7�Ĵ�������Ϊȱʡֵ
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);           //��������ǰװ��ʵ��RAM��ַ
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart3_Handle.USARTx_TX_BUF; //���÷��ͻ������׵�ַ
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                       //���ݴ��䷽�򣬴��ڴ淢�͵�����
    DMA1_InitStructure.DMA_BufferSize = USART3_MAX_SEND_LEN;                  // DMAͨ����DMA����Ĵ�С
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         //�����ַ�Ĵ�������
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  //�ڴ��ַ�Ĵ�������
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                            //����������ģʽ
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                      // DMAͨ�� xӵ�и����ȼ�
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                             // DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel2, &DMA1_InitStructure);

    // DMA1ͨ��3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;  // NVICͨ������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ���

    // DMA1ͨ��2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;  // NVICͨ������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ���

    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE); //��USART3 Rx DMA�ж�
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE); //��USART3 Tx DMA�ж�
    DMA_Cmd(DMA1_Channel3, ENABLE);                 //ʹDMAͨ��3ֹͣ����
                                                    // �ʼ�:û����Ҫ����ʱ�رշ���DMA
    DMA_Cmd(DMA1_Channel2, DISABLE);                //ʹDMAͨ��2ֹͣ����

    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); //��������DMA����
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char *format, ...) //...��ʾ�ɱ����������ɱ�������һ���б�������ר�ŵ�ָ��ָ�����������޶�����������
{
    va_list arg_ptr; //ʵ�����ɱ䳤�����б�

    while (Usart3_Handle.USARTx_TX_FLAG)
        ; //�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�

    va_start(arg_ptr, format); //��ʼ���ɱ�����б�����formatΪ�ɱ䳤�б����ʼ�㣨��һ��Ԫ�أ�

    // USART2_MAX_TX_LEN+1�ɽ��ܵ�����ַ���(���ֽ�����UNICODEһ���ַ������ֽ�), ��ֹ��������Խ��
    vsnprintf((char *)Usart3_Handle.USARTx_TX_BUF, USART3_MAX_SEND_LEN + 1, format, arg_ptr); //��USART2_TX_BUF���׵�ַ��ʼƴ�ϣ�ƴ��format���ݣ�USART2_MAX_TX_LEN+1���Ƴ��ȣ���ֹ��������Խ��

    va_end(arg_ptr); //ע�����ر�

    DMA_USART3_Tx_Data(Usart3_Handle.USARTx_TX_BUF, strlen((const char *)Usart3_Handle.USARTx_TX_BUF)); //����USART2_TX_BUF����
}

/*����һ���ֽ�����*/
void u3_send_byte(unsigned char SendData)
{
    USART_SendData(USART3, SendData);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
        ;
}

/*����һ���ֽ�����*/
unsigned char u3_get_byte(unsigned char *GetData)
{
    if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
    {
        return 0; //û���յ�����
    }
    *GetData = USART_ReceiveData(USART3);
    return 1; //�յ�����
}

// DMA ����Ӧ��Դ��
void DMA_USART3_Tx_Data(u8 *buffer, u32 size)
{
    while (Usart3_Handle.USARTx_TX_FLAG)
        {
            printf("11\r\n");
        };                                   //�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�
    Usart3_Handle.USARTx_TX_FLAG = 1;       // USART2���ͱ�־���������ͣ�
    DMA1_Channel2->CMAR = (uint32_t)buffer; //����Ҫ���͵����ݵ�ַ
    DMA1_Channel2->CNDTR = size;            //����Ҫ���͵��ֽ���Ŀ
    DMA_Cmd(DMA1_Channel2, ENABLE);         //��ʼDMA����
}

// DMA1ͨ��2�ж�
void DMA1_Channel2_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC2) != RESET) // DMA������ɱ�־
    {
        DMA_ClearITPendingBit(DMA1_IT_TC2);     //����жϱ�־
        USART_ClearFlag(USART3, USART_FLAG_TC); //�������3�ı�־λ
        DMA_Cmd(DMA1_Channel2, DISABLE);        //�ر�USART3 TX DMA1 ��ָʾ��ͨ��
        Usart3_Handle.USARTx_TX_FLAG = 0;       // USART3���ͱ�־(�ر�)
    }
}
