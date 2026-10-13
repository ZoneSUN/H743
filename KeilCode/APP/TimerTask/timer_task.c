/** @file timer_task.c
 *  @brief 周期定时器通知和主循环任务函数执行。 */
#include "timer_task.h"
#include "bsp_TIMx.h"
#include <stdio.h>

#define TIMER_TASK_COUNT 6U

typedef struct
{
    /* 周期和已累计 tick 均以 TIM1 的基本周期为单位。 */
    uint32_t period_ticks;
    uint32_t elapsed_ticks;
    volatile uint8_t pending;
    void (*execute)(void);
} TimerTaskItem;

/* 任务1：执行任务1的默认示例业务，后续可替换为实际处理函数。 */
static void Task1(void)
{
    printf("task1\r\n");
}

/* 任务2：执行任务2的默认示例业务，后续可替换为实际处理函数。 */
static void Task2(void)
{
    printf("task2\r\n");
}

/* 任务3：执行任务3的默认示例业务，后续可替换为实际处理函数。 */
static void Task3(void)
{
    printf("task3\r\n");
}

/* 任务4：执行任务4的默认示例业务，后续可替换为实际处理函数。 */
static void Task4(void)
{
    printf("task4\r\n");
}

/* 任务5：执行任务5的默认示例业务，后续可替换为实际处理函数。 */
static void Task5(void)
{
    printf("task5\r\n");
}

/* 任务6：执行任务6的默认示例业务，后续可替换为实际处理函数。 */
static void Task6(void)
{
    printf("task6\r\n");
}

/* 六项任务共用 TIM1；周期分别为2、3、4、5、8、10个基本 tick，当前每个 tick 为1秒。 */
static TimerTaskItem s_tasks[TIMER_TASK_COUNT] =
{
    { 2U, 0U, 0U, Task1 },
    { 3U, 0U, 0U, Task2 },
    { 4U, 0U, 0U, Task3 },
    { 5U, 0U, 0U, Task4 },
    { 8U, 0U, 0U, Task5 },
    { 10U, 0U, 0U, Task6 }
};

/**
 * @brief HAL定时器更新回调，累计 TIM1 基本周期并置位到期任务通知。
 * @param handle 触发回调的 HAL 定时器句柄。
 * @retval 无。
 * @note 回调运行在中断上下文，仅做计数和置位通知，不执行任务函数。
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *handle)
{
    uint32_t index;

    if (handle != &tim_task_handle) { return; }

    for (index = 0U; index < TIMER_TASK_COUNT; ++index)
    {
        if (s_tasks[index].period_ticks == 0U) { continue; }

        ++s_tasks[index].elapsed_ticks;
        if (s_tasks[index].elapsed_ticks >= s_tasks[index].period_ticks)
        {
            s_tasks[index].elapsed_ticks = 0U;
            s_tasks[index].pending = 1U;
        }
    }
}

/**
 * @brief 在主循环中一次取走六项任务通知并执行对应任务函数。
 * @retval 无。
 * @note 快照和清除通知在临界区内完成，任务函数在退出临界区后执行。
 */
void Timer_Task_Run(void)
{
    uint8_t pending[TIMER_TASK_COUNT];
    uint32_t index;
    uint32_t mask = __get_PRIMASK();

    /* 防止中断在复制和清零之间置位通知，随后恢复调用前的中断状态。 */
    __disable_irq();
    for (index = 0U; index < TIMER_TASK_COUNT; ++index)
    {
        pending[index] = s_tasks[index].pending;
        s_tasks[index].pending = 0U;
    }
    __set_PRIMASK(mask);

    for (index = 0U; index < TIMER_TASK_COUNT; ++index)
    {
        if ((pending[index] != 0U) && (s_tasks[index].execute != NULL))
        {
            s_tasks[index].execute();
        }
    }
}
