/*
 * @Description: µ×°åÀ©Õ¹ 485-3£¬USART1 PB14 TX / PB15 RX
 */
#include "USART1_485.h"

UART_HandleTypeDef usart1_485_handle;
DMA_HandleTypeDef dma_usart1_rx_handle;
DMA_HandleTypeDef dma_usart1_tx_handle;
/* ?? 0x24000480????? UART1 ????????? */
uint8_t usart1_rx_buf[USART1_485_RX_BUFFER_SIZE] __attribute__((at(0x24000480), zero_init));
/* ?? 0x240004A0????? UART1 ????????? */
uint8_t usart1_tx_buf[USART1_485_TX_BUFFER_SIZE] __attribute__((at(0x240004A0), zero_init));

void USART1_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable)
{
  HAL_StatusTypeDef clk_status = USARTx_PeriphCLK_Config(USART1, RCC_USART16CLKSOURCE_D2PCLK2);
  printf("USART1_485_Start status: %d, PCLK2=%luM\r\n",
         (int)clk_status, (unsigned long)HAL_RCC_GetPCLK2Freq() / 1000000U);

  /* PB14 TX / PB15 RX£¬AF4 */
  HAL_StatusTypeDef gpio_status = USARTx_GPIO_Init(GPIOB, GPIOB,
                                                   GPIO_PIN_14, GPIO_PIN_15,
                                                   GPIO_AF4_USART1, GPIO_AF4_USART1);
  USARTx_USART_CLK_Init(USART1);
  HAL_StatusTypeDef uart_status = USARTx_Init(&usart1_485_handle, USART1, baudrate);
  (void)gpio_status;
  (void)uart_status;

  if ((DMA_tx_Enable != false) || (DMA_rx_Enable != false))
  {
    DMAx_CLK_Init(DMA1);
  }

  if (DMA_rx_Enable != false)
  {
    HAL_StatusTypeDef dma_rx_status = USARTx_DMA_Init(&usart1_485_handle,
                                                      &dma_usart1_rx_handle,
                                                      DMA1_Stream4,
                                                      DMA_REQUEST_USART1_RX,
                                                      DMA_PERIPH_TO_MEMORY);
    printf("USART1 DMA RX status: %d\r\n", (int)dma_rx_status);
  }
  if (DMA_tx_Enable != false)
  {
    HAL_StatusTypeDef dma_tx_status = USARTx_DMA_Init(&usart1_485_handle,
                                                      &dma_usart1_tx_handle,
                                                      DMA1_Stream5,
                                                      DMA_REQUEST_USART1_TX,
                                                      DMA_MEMORY_TO_PERIPH);
    printf("USART1 DMA TX status: %d\r\n", (int)dma_tx_status);
  }
  if ((DMA_rx_Enable != false) && (DMA_tx_Enable != false))
  {
    DMAx_NVIC_Init(DMA1_Stream4_IRQn, 4, 0);
    DMAx_NVIC_Init(DMA1_Stream5_IRQn, 4, 0);
    USARTx_NVIC_Init(USART1_IRQn, 4, 0);
    USARTx_ReceiveToIdle_DMA(&usart1_485_handle, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
  }
  else if ((DMA_rx_Enable != false) && (DMA_tx_Enable == false))
  {
    DMAx_NVIC_Init(DMA1_Stream4_IRQn, 4, 0);
    USARTx_NVIC_Init(USART1_IRQn, 4, 0);
    USARTx_ReceiveToIdle_DMA(&usart1_485_handle, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
  }
  else if ((DMA_rx_Enable == false) && (DMA_tx_Enable != false))
  {
    DMAx_NVIC_Init(DMA1_Stream5_IRQn, 4, 0);
    USARTx_NVIC_Init(USART1_IRQn, 4, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&usart1_485_handle, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
  }
  else
  {
    USARTx_NVIC_Init(USART1_IRQn, 4, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&usart1_485_handle, usart1_rx_buf, USART1_485_RX_BUFFER_SIZE);
  }
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&usart1_485_handle);
}

void DMA1_Stream4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_usart1_rx_handle);
}

void DMA1_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_usart1_tx_handle);
}
