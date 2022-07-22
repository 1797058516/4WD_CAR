/*
 * @Description: adc.h
 * @Author: TOTHTOT
 * @Date: 2022-07-18 20:53:24
 * @LastEditTime: 2022-07-22 17:44:15
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4轮小车\HARDWARE\ADC\adc.c
 */
#include "adc.h"
#include "tb6612.h"
#include "config.h"
u16 ADC_Value = 0;
/**
 * @name: DMA_Init
 * @msg: dma初始化此DMA从外设读取到内存
 * @param {DMA_Channel_TypeDef*} DMA_CHx
 * @param {u32} cpar 外设地址
 * @param {u32} cmar 存储器地址
 * @param {u16} cndtr 数据传输量
 * @return {*}
 */
void MYDMA_Init(DMA_Channel_TypeDef *DMA_CHx, u32 cpar, u32 cmar, u16 cndtr)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                          //使能DMA传输
    DMA_DeInit(DMA_CHx);                                                        //将DMA的通道1寄存器重设为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;                            // DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar;                                // DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                          //数据传输方向，从外设到内存
    DMA_InitStructure.DMA_BufferSize = cndtr;                                   // DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;                    //内存地址寄存器不变
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //工作在正常缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         // DMA通道 x拥有中优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                // DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA_CHx, &DMA_InitStructure);                                      //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
    DMA_Cmd(DMA_CHx, ENABLE);
}

/**
 * @name: ADC4_Init
 * @msg: 采样DMA方式读取电压值
 * @return {*}
 */
void ADC4_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE); //使能ADC1通道时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                           //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

    // PA4 作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入引脚
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    ADC_DeInit(ADC1); //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                       //模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                  //模数转换工作在循环转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              // ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                             //顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);                                 //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

    MYDMA_Init(DMA1_Channel1, ADC1_DR_ADDRESS, (u32)&Car_1.car_battery_voltage, 1);     //初始化DMA

    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_13Cycles5); //设置通道等级和采样时间13.5个周期
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE); //使能指定的ADC1

    ADC_ResetCalibration(ADC1); //使能复位校准
    // 配置ADC1通道1，2，3，4为55.5个采样周期
    while (ADC_GetResetCalibrationStatus(ADC1))
        ; //等待复位校准结束

    ADC_StartCalibration(ADC1); //开启AD校准

    while (ADC_GetCalibrationStatus(ADC1))
        ; //等待校准结束

    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能指定的ADC1的软件转换启动功能
}


/**
 * @name: output_voltage
 * @msg: 计算一次电池电压
 * @param {u16} value
 * @return {*}
 */
float output_voltage(u16 value)
{
    #if USE_VOLATGE
    float adc = (float) (ADC_MAX_VOLTAGE/4096)*value;
    adc =  adc/VM_M*BATTERY_VOLTAGE;
    #endif
    #if USE_ANGLESENOR
    float adc = (float) (3.3/4096)*value;
    #endif
    return adc;
}

#if USE_ANGLESENOR
/**
 * @name: ADC_To_Angle
 * @msg: 电压和角度成线性关系3.3最大0最小
 * @param {float} dianya
 * @return {*}
 */
float ADC_To_Angle(float dianya)
{
    float angle = 0.0f;
    angle = (dianya/3.3)*360.0f;
    return angle;
}
#endif

