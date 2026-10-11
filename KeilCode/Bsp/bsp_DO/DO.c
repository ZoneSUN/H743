/*
 * @Description: 底板扩展 8 路继电器。通道号 1~8，电平 0 释放，1 吸合。
 */
#include "DO.h"
#include "bsp_USARTx.h"

static const uint16_t s_do_pin[DO_CH_NUM + 1U] =
{
  0U,
  GPIO_PIN_3,
  GPIO_PIN_6,
  GPIO_PIN_7,
  GPIO_PIN_8,
  GPIO_PIN_9,
  GPIO_PIN_10,
  GPIO_PIN_11,
  GPIO_PIN_12
};

static uint16_t DO_PinMask(void)
{
  uint16_t mask = 0U;
  uint8_t ch;

  for (ch = 1U; ch <= DO_CH_NUM; ch++)
  {
    mask |= s_do_pin[ch];
  }
  return mask;
}

void DO_Init(void)
{
  GPIO_InitTypeDef gpio = {0};
  uint16_t mask = DO_PinMask();

  GPIOx_CLK_Init(GPIOH);
  HAL_GPIO_WritePin(GPIOH, mask, GPIO_PIN_RESET);

  gpio.Pin = mask;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &gpio);
}

void DO_Write(uint8_t channel, uint8_t level)
{
  if ((channel == 0U) || (channel > DO_CH_NUM))
  {
    return;
  }
  HAL_GPIO_WritePin(GPIOH, s_do_pin[channel], (level != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void DO_On(uint8_t channel)
{
  DO_Write(channel, 1U);
}

void DO_Off(uint8_t channel)
{
  DO_Write(channel, 0U);
}

void DO_Step(void)
{
  static uint8_t channel = 0U;

  if (channel != 0U)
  {
    DO_Off(channel);
  }
  channel++;
  if (channel > DO_CH_NUM)
  {
    channel = 1U;
  }
  DO_On(channel);
}

uint8_t DO_Read(uint8_t channel)
{
  if ((channel == 0U) || (channel > DO_CH_NUM))
  {
    return 0U;
  }
  return (HAL_GPIO_ReadPin(GPIOH, s_do_pin[channel]) == GPIO_PIN_SET) ? 1U : 0U;
}
