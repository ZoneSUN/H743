/**
  ******************************************************************************
  * 文件名程: bsp_key.h 
  * 作    者: MZXQ嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 
  * 功    能: KEY-API
  ******************************************************************************
  * 说明：
  * 本例程配套MZXQstm32开发板使用。
  * 
  * 淘宝：
  * 论坛：MZXQ
  * 版权归MZXQ嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */

#ifndef __BSP_KEY_H_
#define __BSP_KEY_H_

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* 类型定义 ------------------------------------------------------------------*/

typedef enum
{
  KEY_UP   = 0,
  KEY_DOWN = 1,
}KEYState_TypeDef;

/* 宏定义 --------------------------------------------------------------------*/

/* GPIO初始化定义 */
#define KEY1_RCC_CLK_ENABLE           __HAL_RCC_GPIOE_CLK_ENABLE
#define KEY1_GPIO_PIN                 GPIO_PIN_3
#define KEY1_GPIO                     GPIOE
#define KEY1_DOWN_LEVEL               GPIO_PIN_RESET /* 根据原理图，KEY1按下时引脚为低电平，
                                                        所以这里设置为0 */
#define KEY2_RCC_CLK_ENABLE           __HAL_RCC_GPIOE_CLK_ENABLE
#define KEY2_GPIO_PIN                 GPIO_PIN_4
#define KEY2_GPIO                     GPIOE
#define KEY2_DOWN_LEVEL               GPIO_PIN_RESET /* 根据原理图，KEY2按下时引脚为低电平，
                                                        所以这里设置为0 */

/* 扩展变量 ------------------------------------------------------------------*/

/* 函数声明 ------------------------------------------------------------------*/
void KEY_GPIO_Init(void);
KEYState_TypeDef KEY1_StateRead(void);
KEYState_TypeDef KEY2_StateRead(void);

#endif /* __BSP_KEY_H_ */

/********** (C) COPYRIGHT 2019-2030 MZXQ嵌入式开发团队 *******END OF FILE******/
