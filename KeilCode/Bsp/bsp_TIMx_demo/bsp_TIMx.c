/** @file bsp_TIMx.c
 *  @brief 公共基础定时器的时钟、周期和中断配置。 */
#include "bsp_TIMx.h"

TIM_HandleTypeDef tim_task_handle;

static IRQn_Type TIMx_GetIRQ(TIM_TypeDef *instance)
{
    /* 调用者已完成实例校验，因此默认分支只覆盖已验证的TIM7。 */
    if (instance == TIM1) { return TIM1_UP_IRQn; }
    if (instance == TIM2) { return TIM2_IRQn; }
    if (instance == TIM6) { return TIM6_DAC_IRQn; }
    return TIM7_IRQn;
}

/** @brief 由HAL初始化回调管理所选定时器时钟。 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) { return; }
    if (htim->Instance == TIM1) { __HAL_RCC_TIM1_CLK_ENABLE(); }
    else if (htim->Instance == TIM2) { __HAL_RCC_TIM2_CLK_ENABLE(); }
    else if (htim->Instance == TIM6) { __HAL_RCC_TIM6_CLK_ENABLE(); }
    else if (htim->Instance == TIM7) { __HAL_RCC_TIM7_CLK_ENABLE(); }
}

/** @brief 由HAL反初始化回调关闭所选定时器时钟及其中断。 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
    if (htim == NULL) { return; }
    if (htim->Instance == TIM1)
    {
        HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
        __HAL_RCC_TIM1_CLK_DISABLE();
    }
    else if (htim->Instance == TIM2)
    {
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
    else if (htim->Instance == TIM6)
    {
        HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
        __HAL_RCC_TIM6_CLK_DISABLE();
    }
    else if (htim->Instance == TIM7)
    {
        HAL_NVIC_DisableIRQ(TIM7_IRQn);
        __HAL_RCC_TIM7_CLK_DISABLE();
    }
}

/**
 * @brief 将微秒周期换算为PSC/ARR并调用HAL完成基础定时器初始化。
 * @param handle 要配置的长期HAL句柄。
 * @param instance 定时器实例，仅支持TIM1、TIM2、TIM6、TIM7。
 * @param period_us 更新周期，单位为微秒。
 * @retval HAL_OK 配置成功；HAL_ERROR 参数或周期超出硬件能力。
 */
HAL_StatusTypeDef TIMx_Init(TIM_HandleTypeDef *handle, TIM_TypeDef *instance,
                            uint32_t period_us)
{
    uint32_t hclk, pclk, clock_hz;
    uint64_t total, divider, required, max_count;

    if (handle == NULL || period_us == 0U ||
        (instance != TIM1 && instance != TIM2 && instance != TIM6 && instance != TIM7))
    {
        return HAL_ERROR;
    }
    hclk = HAL_RCC_GetHCLKFreq();
    pclk = (instance == TIM1) ? HAL_RCC_GetPCLK2Freq() : HAL_RCC_GetPCLK1Freq();
    if ((RCC->CFGR & RCC_CFGR_TIMPRE) == 0U)
    {
        clock_hz = (pclk == hclk) ? pclk : 2U * pclk;
    }
    else
    {
        clock_hz = (pclk >= hclk / 4U) ? hclk : 4U * pclk;
    }
    if (clock_hz < 1000000U) { return HAL_ERROR; }
    /* TIMPRE决定APB定时器时钟是否按2倍或4倍计算；优先得到1us计数基准。 */
    total = (uint64_t)period_us * clock_hz / 1000000ULL;
    max_count = (instance == TIM2) ? 0x100000000ULL : 0x10000ULL;
    divider = clock_hz / 1000000U;
    required = (total + max_count - 1ULL) / max_count;
    /* 16位定时器和32位TIM2分别限制ARR范围，长周期按整数倍扩大分频。 */
    if (required > divider)
    {
        divider = ((required + divider - 1ULL) / divider) * divider;
    }
    if (divider > 65536ULL || total / divider == 0ULL) { return HAL_ERROR; }

    handle->Instance = instance;
    handle->Init.Prescaler = (uint32_t)(divider - 1ULL);
    handle->Init.CounterMode = TIM_COUNTERMODE_UP;
    handle->Init.Period = (uint32_t)(total / divider - 1ULL);
    handle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    handle->Init.RepetitionCounter = 0U;
    handle->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    return HAL_TIM_Base_Init(handle);
}

/**
 * @brief 配置并启动唯一的周期定时器实例。
 * @param instance 定时器实例，仅支持TIM1、TIM2、TIM6、TIM7。
 * @param period_us 更新周期，单位为微秒。
 * @param preempt_priority 抢占优先级，范围为0~15。
 * @param sub_priority 子优先级；当前工程分组下必须为0。
 * @retval HAL_OK 启动成功；HAL_BUSY 已有实例运行；HAL_ERROR 启动失败。
 */
HAL_StatusTypeDef TIMx_Start(TIM_TypeDef *instance, uint32_t period_us,
                             uint32_t preempt_priority, uint32_t sub_priority)
{
    IRQn_Type irq;
    HAL_StatusTypeDef status;

    if ((instance != TIM1 && instance != TIM2 && instance != TIM6 && instance != TIM7) ||
        period_us == 0U || preempt_priority > 15U || sub_priority != 0U)
    {
        return HAL_ERROR;
    }
    if (tim_task_handle.Instance != NULL) { return HAL_BUSY; }
    irq = TIMx_GetIRQ(instance);
    status = TIMx_Init(&tim_task_handle, instance, period_us);
    if (status != HAL_OK)
    {
        if (tim_task_handle.Instance != NULL)
        {
            (void)HAL_TIM_Base_DeInit(&tim_task_handle);
        }
        tim_task_handle.Instance = NULL;
        return status;
    }
    /* HAL初始化可能产生更新标志，启动前清除标志和NVIC pending。 */
    __HAL_TIM_CLEAR_FLAG(&tim_task_handle, TIM_FLAG_UPDATE);
    HAL_NVIC_ClearPendingIRQ(irq);
    status = HAL_TIM_Base_Start_IT(&tim_task_handle);
    if (status != HAL_OK)
    {
        (void)HAL_TIM_Base_DeInit(&tim_task_handle);
        tim_task_handle.Instance = NULL;
        return status;
    }
    HAL_NVIC_SetPriority(irq, preempt_priority, sub_priority);
    HAL_NVIC_EnableIRQ(irq);
    return HAL_OK;
}
