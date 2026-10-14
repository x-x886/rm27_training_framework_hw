//
// Created by cosmosmount on 2025/9/2.
//

#include "bsp_pwm.hpp"

void PWM_Init(void)
{
    // TODO: 根据实际使用的定时器和通道完成 PWM 初始化。
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
}

void PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    // TODO: 调用 HAL 启动指定定时器通道的 PWM，并检查返回状态。
    HAL_TIM_PWM_Start(htim1, TIM_CHANNEL_1);
}

void PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    // TODO: 调用 HAL 停止指定定时器通道的 PWM。
    HAL_TIM_PWM_Stop(htim, Chanel);
}

void PWM_SetPeriod(TIM_HandleTypeDef *htim, float period_s)
{
    // TODO: 根据定时器时钟和预分频值，将秒转换为 ARR 并更新周期。
    uint32_t arr = (unit32_t)(period_s* 1000000.0f) - 1
    __HAL_TIM_SET_SET_AUTORELOAD(htim, arr);
}

void PWM_SetDutyRatio(TIM_HandleTypeDef *htim, float dutyratio, uint32_t channel)
{
    // TODO: 将 [0, 1] 占空比换算为比较值，写入指定通道。
    if (dutyratio > 1.0f) dutyratio = 1.0f;
    if (dutyratio < 0.0f) dutyratio = 0.0f;
    uint32_t ccr = (uint32_t)(dutyratio * (arr + 1));
    __HAL_TIM_SET_COMPARE(htim, channel, ccr);
}
