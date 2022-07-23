#include "delay.h"
#include "usart2.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"
#include "queue.h"

/* ��ATGM336Hͨ�� */
/* //���ڽ��ջ�����
u8 USART2_RX_BUF1[USART2_MAX_RECV_LEN]; //���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 USART2_RX_BUF2[USART2_MAX_RECV_LEN]; //���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];  //���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART2_RX_STA = 0;
 */
FIFO_DMA_H Usart2_Handle;
extern xSemaphoreHandle GPS_Bin_Semaphore;
void USART2_IRQHandler(void)
{
    u8 *p;
    u8 USART2_RX_LEN = 0;                                  //�������ݳ���
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //����2�����ж�
    {
        USART_ReceiveData(USART2);                                  //�������2�����ж�IDLE��־λ
        USART_ClearFlag(USART2, USART_FLAG_TC);                     //���USART2��־λ
        DMA_Cmd(DMA1_Channel6, DISABLE);                            //�ر�USART2 TX DMA1 ��ָʾ��ͨ��
        USART2_RX_LEN = USART2_MAX_RECV_LEN - DMA1_Channel6->CNDTR; //��ý��յ����ֽ���
        if (Usart2_Handle.witchbuf)                                 //֮ǰ�õ�u2rxbuf���л�Ϊu1rxbuf
        {
            p = Usart2_Handle.USARTx_RX_BUF2;                        //�ȱ���ǰһ�����ݵ�ַ���л�������
            DMA1_Channel6->CMAR = (u32)Usart2_Handle.USARTx_RX_BUF1; //�л�Ϊu1rxbuf��������ַ
            Usart2_Handle.witchbuf = 0;                              //��һ���л�Ϊu2rxbuf
        }
        else //֮ǰ�õ�u1rxbuf���л�Ϊu2rxbuf
        {
            p = Usart2_Handle.USARTx_RX_BUF1;                        //�ȱ���ǰһ�����ݵ�ַ���л�������
            DMA1_Channel6->CMAR = (u32)Usart2_Handle.USARTx_RX_BUF2; //�л�Ϊu2rxbuf��������ַ
            Usart2_Handle.witchbuf = 1;                              //��һ���л�Ϊu1rxbuf
        }
        DMA1_Channel6->CNDTR = USART2_MAX_RECV_LEN; // DMAͨ����DMA����Ĵ�С
        DMA_Cmd(DMA1_Channel6, ENABLE);             //ʹ��USART2 TX DMA1 ��ָʾ��ͨ��

        //******************�������������������ݴ������������******************//

        DMA_USART2_Tx_Data(p, USART2_RX_LEN);

        //******************�������������������ݴ������������******************//
    }
}

//��ʼ��IO ����2 DMA��ʽ���ͽ���
// pclk1:PCLK1ʱ��Ƶ��(Mhz)
// bound:������
void USART2_Init(u32 bound)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //����2ʱ��ʹ��

    USART_DeInit(USART2);                     //��λ����2
                                              // USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);          //��ʼ��PA2

    // USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);        //��ʼ��PA3

    USART_InitStructure.USART_BaudRate = bound;                                     //����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                             //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������2

    USART_Cmd(USART2, ENABLE); //ʹ�ܴ���

    //ʹ�ܽ����ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //�����ж�
                                                   //�����жϿ�������
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); //������⴮�ڿ���״̬�ж�
    USART_ClearFlag(USART2, USART_FLAG_TC);

    //�����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11; //��ռ���ȼ�6
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                            //����ָ���Ĳ�����ʼ��VIC�Ĵ���
    // dma����
    Usart2_DMA_Init();
}

void Usart2_DMA_Init(void)
{
    DMA_InitTypeDef DMA1_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // DMA_USART2_RX  USART2->RAM�����ݴ���
    DMA_DeInit(DMA1_Channel6);                                                 //��DMA��ͨ��6�Ĵ�������Ϊȱʡֵ
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);            //��������ǰװ��ʵ��RAM��ַ
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart2_Handle.USARTx_RX_BUF1; //���ý��ջ������׵�ַ
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                        //���ݴ��䷽�򣬴������ȡ���ڴ�
    DMA1_InitStructure.DMA_BufferSize = USART2_MAX_RECV_LEN;                   // DMAͨ����DMA����Ĵ�С
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //�����ַ�Ĵ�������
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //�ڴ��ַ�Ĵ�������
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                             //����������ģʽ
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                       // DMAͨ�� xӵ�и����ȼ�
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                              // DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel6, &DMA1_InitStructure);

    // DMA_USART2_TX  RAM->USART2�����ݴ���
    DMA_DeInit(DMA1_Channel7);                                                //��DMA��ͨ��7�Ĵ�������Ϊȱʡֵ
    DMA1_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);           //��������ǰװ��ʵ��RAM��ַ
    DMA1_InitStructure.DMA_MemoryBaseAddr = (u32)Usart2_Handle.USARTx_TX_BUF; //���÷��ͻ������׵�ַ
    DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                       //���ݴ��䷽�򣬴��ڴ淢�͵�����
    DMA1_InitStructure.DMA_BufferSize = USART2_MAX_SEND_LEN;                  // DMAͨ����DMA����Ĵ�С
    DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         //�����ַ�Ĵ�������
    DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  //�ڴ��ַ�Ĵ�������
    DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          //���ݿ��Ϊ8λ
    DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;                            //����������ģʽ
    DMA1_InitStructure.DMA_Priority = DMA_Priority_High;                      // DMAͨ�� xӵ�и����ȼ�
    DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;                             // DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel7, &DMA1_InitStructure);

    // DMA1ͨ��6 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;  // NVICͨ������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ���

    // DMA1ͨ��7 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;  // NVICͨ������
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);                           //����ָ���Ĳ�����ʼ��NVIC�Ĵ���

    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); //��USART2 Rx DMA�ж�
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE); //��USART2 Tx DMA�ж�
    DMA_Cmd(DMA1_Channel6, ENABLE);                 //ʹDMAͨ��6ֹͣ����
                                                    // �ʼ�:û����Ҫ����ʱ�رշ���DMA
    DMA_Cmd(DMA1_Channel7, DISABLE);                //ʹDMAͨ��7ֹͣ����

    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //��������DMA����
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char *format, ...)
{

    // //VA_LIST ����C�����н����������һ��꣬����ͷ�ļ���#include <stdarg.h>�����ڻ�ȡ��ȷ�������Ĳ�����
    va_list arg_ptr; //ʵ�����ɱ䳤�����б�

    while (Usart2_Handle.USARTx_TX_FLAG)
        ; //�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�

    va_start(arg_ptr, format); //��ʼ���ɱ�����б�����formatΪ�ɱ䳤�б����ʼ�㣨��һ��Ԫ�أ�

    // USART2_MAX_TX_LEN+1�ɽ��ܵ�����ַ���(���ֽ�����UNICODEһ���ַ������ֽ�), ��ֹ��������Խ��
    vsnprintf((char *)Usart2_Handle.USARTx_TX_BUF, USART2_MAX_SEND_LEN + 1, format, arg_ptr); //��USART2_TX_BUF���׵�ַ��ʼƴ�ϣ�ƴ��format���ݣ�USART2_MAX_TX_LEN+1���Ƴ��ȣ���ֹ��������Խ��

    va_end(arg_ptr); //ע�����ر�

    DMA_USART2_Tx_Data(Usart2_Handle.USARTx_TX_BUF, strlen((const char *)Usart2_Handle.USARTx_TX_BUF)); //����USART2_TX_BUF����
}

/**
 * @name: DMA_USART2_Tx_Data
 * @msg: DMA ����Ӧ��Դ�� ��sprintfƴ��buffer
 * @return {*}
 */
void DMA_USART2_Tx_Data(u8 *buffer, u32 size)
{
    while (Usart2_Handle.USARTx_TX_FLAG)
        {
            printf("11\r\n");
        }                                   //�ȴ���һ�η�����ɣ�USART2_TX_FLAGΪ1�����ڷ������ݣ�
    Usart2_Handle.USARTx_TX_FLAG = 1;       // USART2���ͱ�־���������ͣ�
    DMA1_Channel7->CMAR = (uint32_t)buffer; //����Ҫ���͵����ݵ�ַ
    DMA1_Channel7->CNDTR = size;            //����Ҫ���͵��ֽ���Ŀ
    DMA_Cmd(DMA1_Channel7, ENABLE);         //��ʼDMA����
}

// DMA1ͨ��7�ж�
void DMA1_Channel7_IRQHandler(void)
{
    printf("33\r\n");
    if (DMA_GetITStatus(DMA1_IT_TC7) != RESET) // DMA������ɱ�־
    {
        printf("22\r\n");
        DMA_ClearITPendingBit(DMA1_IT_TC7);     //����жϱ�־
        USART_ClearFlag(USART2, USART_FLAG_TC); //�������2�ı�־λ
        DMA_Cmd(DMA1_Channel7, DISABLE);        //�ر�USART2 TX DMA1 ��ָʾ��ͨ��
        Usart2_Handle.USARTx_TX_FLAG = 0;       // USART2���ͱ�־(�ر�)
    }
}