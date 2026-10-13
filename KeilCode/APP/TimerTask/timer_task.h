/** @file timer_task.h
 *  @brief 基于 TIM1 基本周期的六项定时任务处理接口。 */
#ifndef TIMER_TASK_H
#define TIMER_TASK_H

/**
 * @brief 在主循环中处理六项任务的定时器中断通知。
 * @note 任务周期以 TIM1 基本周期 tick 为单位，通知快照后再执行打印。
 */
void Timer_Task_Run(void);

#endif
