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
#include "bsp_i2c.h"       /* 本板硬件 I2C2 总线实例 g_bsp_i2c2。 */
#include "param_manager.h" /* PID 类型、默认值、EEPROM 参数保存和加载接口。 */
/* 私有参数存储对象 ----------------------------------------------------------*/
/* 绑定关系：参数管理对象 -> 24C02 器件对象 -> 本板 I2C2 总线对象。
 * 对象使用 static，生命周期覆盖整个程序，传给驱动的指针始终有效。
 * 此处只描述器件连接和超时，真正的 GPIO/时钟/HAL 初始化在 main 中执行。 */
static at24c02_t s_param_eeprom = {
    &g_bsp_i2c2,                     /* 使用 I2C2：PH4=SCL、PH5=SDA。 */
    AT24C02_ADDRESS_DEFAULT,          /* 7 位器件地址 0x50，底层交给 HAL 前左移。 */
    AT24C02_TIMEOUT_DEFAULT_MS,       /* 每次数据传输的超时，默认 100 ms。 */
    AT24C02_WRITE_WAIT_DEFAULT_MS     /* 每个字节内部写周期的轮询预算，默认 20 ms。 */
};
/* PID 地址由 param_manager.h 的 PARAM_PID_*_ADDR 宏统一定义；共占用 0x00~0x17。
 * 相对偏移：kp=0、ki=4、kd=8、target=12、output_min=16、output_max=20。
 * 0x50 是总线上选择芯片的地址，0x00 是芯片内部存储地址，两者含义不同。 */
static param_manager_t s_param_manager = {&s_param_eeprom};

/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{

  /* 分别保存待写值与实际读回值，避免把写入变量当成读取结果输出。
   * pid_write 由 get_defaults 完整赋值；pid_read 仅在 load 成功后使用。 */
  param_pid_t pid_write; /* 本次启动要保存的六个 PID 浮点参数。 */
  /*
  param_manager_save_pid()
    ↓
param_manager_write_float()
    ↓
at24c02_write()
    ↓
at24c02_write_byte()
    ↓
bsp_i2c_mem_write()
    ↓
HAL_I2C_Mem_Write()        硬件 I2C2 发送数据
*/
  param_pid_t pid_read;  /* 从 EEPROM 加载且通过数值合法性检查的参数。 */
	/*
  main()
    ↓
param_manager_load_pid()    按地址宏读取六个 PID 字段
    ↓
param_manager_read_float()  读取四字节，还原为 float
    ↓
at24c02_read()              检查器件配置和存储地址范围
    ↓
bsp_i2c_mem_read()          检查总线状态，封装 HAL 调用
    ↓
HAL_I2C_Mem_Read()          通过硬件 I2C2 读取 24C02
*/


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


    /* 一次性启动测试：初始化/探测 -> 取默认值 -> 保存/验证 -> 加载 -> 打印。
     * 每次复位都会覆盖 0x00~0x17，不应放入 while 循环反复写入。
     * 本测试验证当前启动中的读写；验证掉电保持时需禁用写入，仅加载旧值。
     * 所有下列状态接口均返回 0 成功、1 失败，通过分支保证失败后不继续处理。
     * EEPROM 阻塞操作及 ACK polling 依赖已初始化的系统时钟和 HAL tick。 */
    /* 第 1 步拆成初始化和地址探测，明确区分软件配置失败与器件未应答。
     * 与 param_manager_init 内部顺序一致；仅为上板测试增加诊断输出。 */
    if (at24c02_init(&s_param_eeprom) != 0U)
    {
        printf("24c02-init failed,status=%u,error=0x%08lX,PCLK1=%lu,I2C2CLK=%lu\r\n",
               (unsigned int)bsp_i2c_get_init_status(&g_bsp_i2c2),
               (unsigned long)bsp_i2c_get_last_error(&g_bsp_i2c2),
               (unsigned long)HAL_RCC_GetPCLK1Freq(),
               (unsigned long)HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2C2));
    }
    else if (at24c02_check(&s_param_eeprom) != 0U)
    {
        /* 探测失败时尚未写入参数。记录 HAL 状态、错误位与两根总线的电平，
         * 便于检查器件地址、接线和上拉；单次电平采样不能替代波形测量。 */
        printf("24c02-probe failed,addr=0x%02X,status=%u,error=0x%08lX,SCL=%u,SDA=%u\r\n",
               (unsigned int)s_param_eeprom.address_7bit,
               (unsigned int)bsp_i2c_get_last_status(&g_bsp_i2c2),
               (unsigned long)bsp_i2c_get_last_error(&g_bsp_i2c2),
               (unsigned int)HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_4),
               (unsigned int)HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_5));
    }
    else
    {
        /* 第 2 步：仅将默认值复制到 RAM，不进行 EEPROM 访问。
         * 默认值：kp=1.50、ki=0.20、kd=0.05、target=100、min=0、max=1000。
         * 如需测试其他参数，可在下一步保存之前修改 pid_write 的对应字段。 */
        param_manager_get_defaults(&pid_write);
        /* 第 3 步：检查参数合法性，编码 24 字节，逐字节写入并等待写完成，
         * 最后读回并比较字节。失败时可能已经写入部分数据，接口不会回滚。 */
        if (param_manager_save_pid(&s_param_manager, &pid_write) != 0U)
        {
            printf("24c02-write/verify failed\r\n");
        }
        /* 第 4 步：保存成功后再独立加载一次，将存储字节解码为六个 float。
         * load 检查 NaN/Inf 及上下限关系；失败时不修改 pid_read，也不打印其值。 */
        else if (param_manager_load_pid(&s_param_manager, &pid_read) != 0U)
        {
            printf("24c02-read failed\r\n");
        }
        else
        {
            /* 第 5 步：只打印实际加载成功的 pid_read，前缀统一为 "24c02-"。
             * %.3f 保留三位小数，只影响显示精度，EEPROM 仍保存完整 float 位模式。
             * float 在 printf 可变参数中自动提升为 double，此处显式转换说明约定。
             * \r\n 表示串口换行；成功输出示例：
             * 24c02-kp=1.500,ki=0.200,kd=0.050,target=100.000,output_min=0.000,output_max=1000.000
             */
            printf("24c02-kp=%.3f,ki=%.3f,kd=%.3f,target=%.3f,output_min=%.3f,output_max=%.3f\r\n",
                   (double)pid_read.kp, (double)pid_read.ki, (double)pid_read.kd,
                   (double)pid_read.target, (double)pid_read.output_min,
                   (double)pid_read.output_max);
        }
    }



  printf("System_Start: V1.0.0\r\n");

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

