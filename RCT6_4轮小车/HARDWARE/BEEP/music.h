/*
 * @Description: 
 * @Author: TOTHTOT
 * @Date: 2022-07-20 18:43:59
 * @LastEditTime: 2022-07-20 18:46:14
 * @LastEditors: TOTHTOT
 * @FilePath: \USERe:\Learn\stm32\实例\RCT6_4轮小车\HARDWARE\BEEP\music.h
 */
enum Low_frequency
{
    l_dao = 262,
    l_re = 286,
    l_mi = 311,
    l_fa = 349,
    l_sao = 392,
    l_la = 440,
    l_xi = 494
};
enum Normal_frequency
{
    dao = 523,
    re = 587,
    mi = 659,
    fa = 698,
    sao = 784,
    la = 880,
    xi = 987
};
enum High_frequency
{
    h_dao = 1046,
    h_re = 1174,
    h_mi = 1318,
    h_fa = 1396,
    h_sao = 1567,
    h_la = 1760,
    h_xi = 1975
};
//以下是《渴望》片头曲的一段简谱“好人一生平安”
const unsigned int f[] = {re, mi, re, dao, l_la, dao, l_la, //每行对应一小节音调
                    l_sao, l_mi, l_sao, l_la, dao,
                    l_la, dao, sao, la, mi, sao,
                    re,
                    mi, re, mi, sao, mi,
                    l_sao, l_mi, l_sao, l_la, dao,
                    l_la, l_la, dao, l_la, l_sao, l_re, l_mi,
                    l_sao,
                    re, re, sao, la, sao,
                    fa, mi, sao, mi,
                    la, sao, mi, re, mi, l_la, dao,
                    re,
                    mi, re, mi, sao, mi,
                    l_sao, l_mi, l_sao, l_la, dao,
                    l_la, dao, re, l_la, dao, re, mi,
                    re,
                    l_la, dao, re, l_la, dao, re, mi,
                    re,
                    0xff}; //以0xff作为音调的结束标志
//以下是简谱中每个音调的节拍
//“4”对应4个延时单位,“2”对应两个延时单位,“1”对应1个延时单位
unsigned char JP[] = {4, 1, 1, 4, 1, 1, 2, //每行对应一小节音调的节拍
                      2, 2, 2, 2, 8,
                      4, 2, 3, 1, 2, 2,
                      10,
                      4, 2, 2, 4, 4,
                      2, 2, 2, 2, 4,
                      2, 2, 2, 2, 2, 2, 2,
                      10,
                      4, 4, 4, 2, 2,
                      4, 2, 4, 4,
                      4, 2, 2, 2, 2, 2, 2,
                      10,
                      4, 2, 2, 4, 4,
                      2, 2, 2, 2, 6,
                      4, 2, 2, 4, 1, 1, 4,
                      10,
                      4, 2, 2, 4, 1, 1, 4,
                      10};
					  
					  
					  
					  
					  
					  
					  
