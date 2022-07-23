/*
 * @Description: adc.h
 * @Author: TOTHTOT
 * @Date: 2022-07-18 20:53:38
 * @LastEditTime: 2022-07-19 19:49:37
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\ADC\adc.h
 */
#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
#include "sys.h"
#define ADC1_DR_ADDRESS ((u32)0x4001244C)
#define BATTERY_VOLTAGE 12.6
#define ADC_MAX_VOLTAGE 3.3
#define VM_M 2.52
extern u16 ADC_Value;

void ADC4_Init(void);
u16 Get_Adc_Average(u8 ch,u8 times);
float output_voltage(u16 value);

#endif /* __ADC_H */
