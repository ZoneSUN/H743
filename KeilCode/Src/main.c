/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-28
 *
 * @copyright Copyright (c) 2026
 *
 */

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "string.h"
#include "bsp_debug_usart.h"
#include "USART2_485.h"
#include "USART2_485_RxTx.h"
#include "UART7_485.h"
#include "USART1_485.h"
#include "UART4_485.h"
#include "CAN2.h"
#include "DO.h"
#include "BRT38Encoder_Process.h"
#include "sys.h"
#include "bsp_i2c.h"       /* 本板硬件I2C1/I2C2启动入口。 */
#include "at24c02.h" /* 24C02 初始化、字符串写入与读取测试。 */
#include "param_manager.h"


/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
	
	
	
int main(void)
{
//测试定义参数
  const uint8_t text[] = "123123";
  uint8_t readback[sizeof(text)] = {0};
	float kp_read = 0.0F;
  param_pid_t pid_read;

  MPU_Config(); // 配置MPU
  sys_cache_enable(); // 使能CPU L1-Cache
  SCB_EnableDCache(); // 使能数据缓存

  /* 复位所有外设，初始化Flash接口和系统滴答定时器(HSI) */
  HAL_Init(); //初始化HAL库
  /* 配置系统时钟 */
  sys_stm32_clock_init(160, 5, 2, 2);       /* 设置时钟, 400Mhz */

  /* printf 重定向至调试 USART1：115200 baud，PA9=TX、PA10=RX。
   * 必须先初始化串口，再执行测试，否则无法观察测试状态。 */
  DEBUG_USARTx_Init();




  printf("System_Start: V1.0.0\r\n");

  /* USER CODE BEGIN EEPROM_Test */

  /* 已完成HAL、时钟和串口初始化，启动板载EEPROM使用的I2C2。 */
  bsp_I2C2_Start();
  /* 0x20是内部存储地址；写入含结尾NUL的10字节。
   * 读取9个字符，最后一字节保持0；只在读写都成功后打印实际读回数据。
   * 每次启动覆盖测试区，验证掉电保存时移除write调用。 */
  at24c02_write(AT24C02_TEST_ADDRESS, text, sizeof(text));					                                         //字符写入测试
  at24c02_read(AT24C02_TEST_ADDRESS, readback, sizeof(readback));		                                         //字符读取测试
	//at24c02_write(PARAM_PID_KP_ADDR,  (const uint8_t *)&PARAM_PID_DEFAULTS.kp, sizeof(PARAM_PID_DEFAULT_KP));  //float写入测试
  param_manager_save_pid(&PARAM_PID_DEFAULTS);  
  param_manager_load_pid(&pid_read);                                                              //pid整体读取测试
  //at24c02_read(PARAM_PID_KI_ADDR, (uint8_t *)&kp_read, sizeof(kp_read)) == 0U;                                 //float读取测试
    printf("kp=%.3f, ki=%.3f, kd=%.3f, target=%.3f, min=%.3f, max=%.3f\r\n",
           (double)pid_read.kp,
           (double)pid_read.ki,
           (double)pid_read.kd,
           (double)pid_read.target,
           (double)pid_read.output_min,
           (double)pid_read.output_max);
    //printf("EEPROM kp=%.3f\r\n", (double)kp_read);

  
  /* USER CODE END EEPROM_Test */

  UART7_485_Start(9600U, true, true); //485_1 初始化UART7_485

  USART2_485_Start(115200U, true, true); //485_2 初始化USART2_485

  //USART1_485_Start(115200U, true, true); //485_3 初始化USART1_485

  UART4_485_Start(115200U, true, true); //485_4 初始化UART4_485

  CAN1_Start(500000U); // 初始化CAN1，波特率 500kbps

  CAN2_Start(500000U); // 初始化CAN2，波特率 500kbps

  DO_Init(); /* 8 路继电器输出低 */

  BRT38_Job_BindDefault();

  printf("__LINE__: %d\r\n", __LINE__);

	/* 无限循环 */
  while (1)
  {
    /*
    HAL_UART_Transmit_DMA (&uart7_485_handle, "uart7_485_handle", strlen("uart7_485_handle"));
    HAL_UART_Transmit_DMA (&usart2_485_handle, "usart2_485_handle", strlen("usart2_485_handle"));
    HAL_UART_Transmit_DMA (&usart1_485_handle, "usart1_485_handle", strlen("usart1_485_handle"));
    HAL_UART_Transmit_DMA (&uart4_485_handle, "usart4_485_handle", strlen("usart4_485_handle"));
		*/
    /* CAN1_Send 接口未实现，暂时禁用 */
    // CAN1_Send(0x112U, 0x112U, 0x112U, 0x112U);
    CAN2_Send(0x112U, 0x112U, 0x112U, 0x112U);
    //DO_Step();
    HAL_Delay(1000);
  }

}

