/*
 * @Description: beep.c ��Դ������ �͵�ƽ�������һ��10us���ж�ʹ��
 * @Author: TOTHTOT
 * @Date: 2022-07-18 20:55:19
 * @LastEditTime: 2022-07-24 19:04:00
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4��С��\HARDWARE\BEEP\beep.c
 */
#include "beep.h"
#include "delay.h"
#include "timer.h"
#include "usart.h"
S_BEEP_Handle Beep;
//              ��Si Do Re  Mi  Fa So  La  Si ?��Do?��Re?��Mi?��Fa?��So ��
uc16 tone[] = {247, 262, 294, 330, 349, 392, 440, 294, 523, 587, 659, 698, 784, 1000};
/**
 * @name: Beep_Init
 * @msg: ��ʼ����Դ�������͵�ƽ����
 * @return {*}
 */
void Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��PA,PD�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;        // BEEP-->PB.15 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);            //�����趨������ʼ��GPIOB.15
    GPIO_SetBits(GPIOB, GPIO_Pin_15);                 // PB.15 �����
    Timer1_Init(10 - 1, 72 - 1);                      //��ʼ����ʱ��1,10us���һ��,���Ƶ��100KHz
}

/**
 * @name: Beep_Voice
 * @msg: ����һ��Ƶ�ʵ�����
 * @param {S_BEEP_Handle} *b beep�Ľṹ��ָ��
 * @param {u8} volume ����
 * @param {float} Hz Ƶ��
 * @param {u16} jp ����,��������ʱ�䵥λms
 * @return {*}
 */
void Beep_Voice(S_BEEP_Handle *b, u8 volume, float Hz, u16 jp)
{
    u16 h_time;

    b->beep_en = beep_enable;
    b->pinlv = (float)1.0 / Hz * 1000000.0;   //ת����us��
    h_time = b->pinlv * (100 - volume) / 100; //�������õ��������øߵ�ƽ����ʱ��ԽС����Խ��
    b->high_time = h_time;
    b->pinlv_m = b->pinlv;
    b->jiepai = (int)((jp * 1000) / (1 / Hz * 1000000));
    // printf("pl:%f\r\n", b->pinlv);
    printf("vo:%d, jp:%d\r\n", h_time, (int)((jp * 1000) / (1 / Hz * 1000000)));
    BEEP_ON; //ʹ�ܶ�ʱ��
}

/**
 * @name: Beep_Do
 * @msg: ����Do��
 * @param {u8} volume ����
 * @param {u16} jp ����ʱ��
 * @return {*}
 */
void Beep_Do(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 882, 500);
}

/**
 * @name: Beep_Re
 * @msg: ����Re��
 * @param {u8} volume ����
 * @param {u16} jp ����ʱ��
 * @return {*}
 */
void Beep_Re(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 990, 500);
}

/**
 * @name: Beep_Mi
 * @msg: ����Mi��
 * @param {u8} volume ����
 * @param {u16} jp ����ʱ��
 * @return {*}
 */
void Beep_Mi(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 525, 500);
}

/**
 * @name: Beep_Fa
 * @msg: ����Fa��
 * @param {u8} volume ����
 * @param {u16} jp ����ʱ��
 * @return {*}
 */
void Beep_Fa(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 589, 500);
}

/**
 * @name: Beep_So
 * @msg: ����So��
 * @param {u8} volume ����
 * @param {u16} jp ����ʱ��
 * @return {*}
 */
void Beep_So(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 661, 500);
}

/**
 * @name: Beep_La
 * @msg: ����La��
 * @param {u8} volume ����
 * @param {u16} jp ����ʱ��
 * @return {*}
 */
void Beep_La(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 700, 500);
}

/**
 * @name: Beep_Si
 * @msg: ����Si��
 * @param {u8} volume ����
 * @param {u16} jp ����ʱ��
 * @return {*}
 */
void Beep_Si(u8 volume, u16 jp)
{
    Beep_Voice(&Beep, volume, 786, 500);
}

/**
 * @name: Show_Voice
 * @msg: ����߸�����
 * @return {*}
 */
void Show_Voice(void)
{
    Beep_Do(80, 500);
    delay_ms(500);
    Beep_Re(80, 500);
    delay_ms(500);
    Beep_Mi(80, 500);
    delay_ms(500);
    Beep_Fa(80, 500);
    delay_ms(500);
    Beep_So(80, 500);
    delay_ms(500);
    Beep_La(80, 500);
    delay_ms(500);
    Beep_Si(80, 500);
    delay_ms(500);
}

/**
 * @name: Boot_prompt_tone
 * @msg: ������ʾ��
 * @return {*}
 */
void Boot_prompt_tone(void)
{
    Beep_Voice(&Beep, 70, 4000, 300);
}
