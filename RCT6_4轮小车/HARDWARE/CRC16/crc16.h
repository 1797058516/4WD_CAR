/*
 * @Description: 
 * @Author: TOTHTOT
 * @Date: 2018-12-05 04:44:46
 * @LastEditTime: 2022-07-25 10:25:35
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Github\4WD_CAR\RCT6_4��С��\HARDWARE\CRC16\crc16.h
 */
#ifndef __CRC16_H
#define __CRC16_H
#include "sys.h"
//////////////////////////////////////////////////////////////////
//��������
extern uint8_t crc16_data[];
//////////////////////////////////////////////////////////////////	
//��������
uint16_t crc16tablefast(uint8_t *ptr, uint16_t len);
#endif
