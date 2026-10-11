/*
 * @Description: 底板扩展继电器 DO，上电输出低
 * 1 PH3，2 PH6，3 PH7，4 PH8，5 PH9，6 PH10，7 PH11，8 PH12
 */
#ifndef __BSP_DO_H__
#define __BSP_DO_H__

#include "stm32h7xx_hal.h"
#include "stdint.h"

#define DO_CH_NUM                           8U

void DO_Init(void);
void DO_Write(uint8_t channel, uint8_t level);
void DO_On(uint8_t channel);
void DO_Off(uint8_t channel);
/* 关掉当前路，吸合下一路。8 路之后回到第 1 路。 */
void DO_Step(void);
uint8_t DO_Read(uint8_t channel);

#endif /* __BSP_DO_H__ */
