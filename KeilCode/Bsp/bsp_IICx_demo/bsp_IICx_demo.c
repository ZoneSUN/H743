/** @file bsp_IICx_demo.c
 * @brief CubeMX 风格的 I2C1/I2C2 初始化和 GPIO/时钟 MSP 配置。
 *
 * I2C1 使用 PB8/PB9（AF4），I2C2 使用 PH4/PH5（AF4）。两组引脚均为
 * 复用开漏，GPIO 不启用内部上拉，通信波形依赖板级外部上拉和线路负载。
 * I2C1/I2C2 共用 I2C123 内核时钟选择域，因此 MSP 配置使用同一来源；
 * 修改任一总线的时钟选择时必须同时审查另一条总线的 TIMING 配置。
 */
#include "bsp_IICx_demo.h"

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

/* 本工程未统一提供 CubeMX Error_Handler；外设、滤波或时钟配置失败时停机，
 * 防止上层把未完成初始化的句柄当作可用总线继续访问。这里是软件停机，
 * 并不代表已经完成任何实际硬件通信测试。 */
static void bsp_i2c_init_error(void)
{
    __disable_irq();
    while (1) { }
}

/**
 * @brief 通用I2C主机初始化，共用一份参数设置代码。
 * @param handle 每条总线自己的HAL句柄，例如&hi2c1或&hi2c2。
 * @param instance 对应硬件外设I2C1或I2C2，必须与传入句柄匹配。
 * @param timing TIMINGR值，由bsp_i2c按实际内核时钟及线路条件指定。
 * @note 调用前完成HAL及系统时钟初始化；失败按原方式停机。
 *       MSP根据Instance选择GPIO和外设时钟，不探测或访问EEPROM。
 * 输入参数：
 *   handle：I2C句柄指针，必须指向有效的I2C_HandleTypeDef结构体。
 *   instance：I2C外设实例指针，例如I2C1或I2C2。
 *   timing：TIMINGR寄存器值，按实际内核时钟和线路条件计算。
 * 输出参数：无。
 * 调用关系：bsp_i2c_init()间接调用。   
 */
void MX_I2C_Init(I2C_HandleTypeDef *handle, I2C_TypeDef *instance, uint32_t timing)
{
    /* USER CODE BEGIN I2C_Init 0 */
    /* USER CODE END I2C_Init 0 */
    handle->Instance = instance;
    handle->Init.Timing = timing;
    /* OwnAddress是MCU自身从机地址，不是EEPROM的0x50。 */
    handle->Init.OwnAddress1 = 0U;
    handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    handle->Init.OwnAddress2 = 0U;
    handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    /* 句柄首次初始化时，HAL通过MSP完成所选实例的时钟和GPIO配置。 */
    if (HAL_I2C_Init(handle) != HAL_OK)
    {
        bsp_i2c_init_error();
    }
    /* 模拟滤波开启；数字滤波为0，不增加额外数字滤波采样。 */
    if (HAL_I2CEx_ConfigAnalogFilter(handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        bsp_i2c_init_error();
    }
    if (HAL_I2CEx_ConfigDigitalFilter(handle, 0U) != HAL_OK)
    {
        bsp_i2c_init_error();
    }
    /* USER CODE BEGIN I2C_Init 2 */
    /* USER CODE END I2C_Init 2 */
}

/**
 * @brief 公共SCL/SDA引脚配置，端口、引脚和复用功能由MSP分支传入。
 * @note 调用前开启对应GPIO时钟；开漏高电平由板上外部上拉提供。
 * 输入参数：
 *   port：GPIO端口，例如GPIOB或GPIOH。
 *   pins：GPIO引脚掩码，例如GPIO_PIN_8|GPIO_PIN_9。
 *   alternate：GPIO复用功能，例如GPIO_AF4_I2C1或GPIO_AF4_I2C2。
 * 输出参数：无。
 * 调用关系：由HAL_I2C_MspInit间接调用。
 */
static void I2C_GPIO_Init(GPIO_TypeDef *port, uint32_t pins, uint32_t alternate)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = pins;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = alternate;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/**
 * @brief HAL首次初始化时调用，准备I2C1/I2C2的时钟和GPIO。
 * @note 共享时钟配置集中写一次，每次回调仍会执行；
 *       分支只保留各自时钟使能及GPIO参数。不处理其他I2C实例。
 * 输入参数：
 *   i2cHandle：待初始化的I2C句柄，必须指向有效的I2C_HandleTypeDef结构体。
 * 输出参数：无。
 * 调用关系：由HAL_I2C_Init间接调用。
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    if (i2cHandle == NULL ||
        (i2cHandle->Instance != I2C1 && i2cHandle->Instance != I2C2))
    {
        return;
    }
    /* USER CODE BEGIN I2C_MspInit 0 */
    /* USER CODE END I2C_MspInit 0 */
    /* 两条总线共用I2C123时钟源；与bsp_i2c传入的100MHz时序约定一致。 */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C123;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        bsp_i2c_init_error();
    }

    if (i2cHandle->Instance == I2C1)
    {
        /* USER CODE BEGIN I2C1_MspInit 0 */
        /* USER CODE END I2C1_MspInit 0 */
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_I2C1_CLK_ENABLE();
        /* PB8=SCL、PB9=SDA，AF4复用。 */
        I2C_GPIO_Init(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_AF4_I2C1);
        /* USER CODE BEGIN I2C1_MspInit 1 */
        /* USER CODE END I2C1_MspInit 1 */
    }
    else if (i2cHandle->Instance == I2C2)
    {
        /* USER CODE BEGIN I2C2_MspInit 0 */
        /* USER CODE END I2C2_MspInit 0 */
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_I2C2_CLK_ENABLE();
        /* PH4=SCL、PH5=SDA，AF4复用，当前板载24C02使用。 */
        I2C_GPIO_Init(GPIOH, GPIO_PIN_4 | GPIO_PIN_5, GPIO_AF4_I2C2);
        /* USER CODE BEGIN I2C2_MspInit 1 */
        /* USER CODE END I2C2_MspInit 1 */
    }
}

/*反初始化I2C总线，释放GPIO和时钟。*/
/* 用途：释放指定 I2C 实例的 GPIO 和外设时钟。
 * 输入：待反初始化的 HAL 句柄；NULL 直接返回。输出：无。
 * 调用关系：由 HAL_I2C_DeInit 间接调用。 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *i2cHandle)
{
    /* HAL_I2C_DeInit 的反向回调；只关闭目标实例时钟并释放其 GPIO。 */
    if (i2cHandle == NULL) { return; }
    if (i2cHandle->Instance == I2C1)
    {
        /* USER CODE BEGIN I2C1_MspDeInit 0 */
        /* USER CODE END I2C1_MspDeInit 0 */
        __HAL_RCC_I2C1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
        /* USER CODE BEGIN I2C1_MspDeInit 1 */
        /* USER CODE END I2C1_MspDeInit 1 */
    }
    else if (i2cHandle->Instance == I2C2)
    {
        /* USER CODE BEGIN I2C2_MspDeInit 0 */
        /* USER CODE END I2C2_MspDeInit 0 */
        __HAL_RCC_I2C2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOH, GPIO_PIN_4 | GPIO_PIN_5);
        /* USER CODE BEGIN I2C2_MspDeInit 1 */
        /* USER CODE END I2C2_MspDeInit 1 */
    }
}
