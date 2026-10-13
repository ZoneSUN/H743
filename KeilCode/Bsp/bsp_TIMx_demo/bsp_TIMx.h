/** @file bsp_TIMx.h
 *  @brief TIM1、TIM2、TIM6、TIM7的公共基础定时器接口。 */
#ifndef BSP_TIMX_H
#define BSP_TIMX_H
#include "stm32h7xx_hal.h"

/* BSP拥有唯一长期句柄；IRQ仅通过该句柄判断当前选中实例并转发。 */
extern TIM_HandleTypeDef tim_task_handle;
/**
 * @brief 配置并启动周期更新中断。
 * @param instance 支持TIM1、TIM2、TIM6、TIM7。
 * @param period_us 周期，单位微秒。
 * @param preempt_priority 抢占优先级，范围0~15。
 * @param sub_priority 子优先级，当前分组下必须为0。
 * @retval HAL_OK成功；HAL_BUSY已有实例运行；HAL_ERROR参数或周期无效。
 */
HAL_StatusTypeDef TIMx_Start(TIM_TypeDef *instance, uint32_t period_us,
                             uint32_t preempt_priority, uint32_t sub_priority);
/**
 * @brief 按指定实例和微秒周期配置BSP长期句柄。
 * @param handle 待配置的HAL句柄。
 * @param instance 支持TIM1、TIM2、TIM6、TIM7。
 * @param period_us 周期，单位微秒。
 * @retval HAL_OK成功；HAL_ERROR参数或周期无效。
 */
HAL_StatusTypeDef TIMx_Init(TIM_HandleTypeDef *handle, TIM_TypeDef *instance,
                            uint32_t period_us);
#endif
