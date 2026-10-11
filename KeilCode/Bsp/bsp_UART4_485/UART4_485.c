/*
 * @Description: 底板扩展 485-4，UART4 PH13 TX / PH14 RX
 */
#include "UART4_485.h"

UART_HandleTypeDef uart4_485_handle;
DMA_HandleTypeDef dma_uart4_rx_handle;
DMA_HandleTypeDef dma_uart4_tx_handle;

/* USART1 TX 缓冲占到 0x240008A0，这里从 32 字节对齐地址继续放 */
uint8_t uart4_rx_buf[UART4_485_RX_BUFFER_SIZE] __attribute__((at(0x240008C0), zero_init));
uint8_t uart4_tx_buf[UART4_485_TX_BUFFER_SIZE] __attribute__((at(0x240008E0), zero_init));

void UART4_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable)
{
  HAL_StatusTypeDef clk_status = USARTx_PeriphCLK_Config(UART4, RCC_USART234578CLKSOURCE_D2PCLK1);
  printf("UART4_485_Start status: %d, PCLK1=%luM\r\n",
         (int)clk_status, (unsigned long)HAL_RCC_GetPCLK1Freq() / 1000000U);

  /* PH13 TX / PH14 RX，AF8 */
  HAL_StatusTypeDef gpio_status = USARTx_GPIO_Init(GPIOH, GPIOH,
                                                   GPIO_PIN_13, GPIO_PIN_14,
                                                   GPIO_AF8_UART4, GPIO_AF8_UART4);
  USARTx_USART_CLK_Init(UART4);
  HAL_StatusTypeDef uart_status = USARTx_Init(&uart4_485_handle, UART4, baudrate);
  (void)gpio_status;
  (void)uart_status;

  if ((DMA_tx_Enable != false) || (DMA_rx_Enable != false))
  {
    DMAx_CLK_Init(DMA1);
  }

  if (DMA_rx_Enable != false)
  {
    HAL_StatusTypeDef dma_rx_status = USARTx_DMA_Init(&uart4_485_handle,
                                                      &dma_uart4_rx_handle,
                                                      DMA1_Stream6,
                                                      DMA_REQUEST_UART4_RX,
                                                      DMA_PERIPH_TO_MEMORY);
    printf("UART4 DMA RX status: %d\r\n", (int)dma_rx_status);
  }
  if (DMA_tx_Enable != false)
  {
    HAL_StatusTypeDef dma_tx_status = USARTx_DMA_Init(&uart4_485_handle,
                                                      &dma_uart4_tx_handle,
                                                      DMA1_Stream7,
                                                      DMA_REQUEST_UART4_TX,
                                                      DMA_MEMORY_TO_PERIPH);
    printf("UART4 DMA TX status: %d\r\n", (int)dma_tx_status);
  }
  if ((DMA_rx_Enable != false) && (DMA_tx_Enable != false))
  {
    DMAx_NVIC_Init(DMA1_Stream6_IRQn, 6, 0);
    DMAx_NVIC_Init(DMA1_Stream7_IRQn, 6, 0);
    USARTx_NVIC_Init(UART4_IRQn, 6, 0);
    USARTx_ReceiveToIdle_DMA(&uart4_485_handle, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
  }
  else if ((DMA_rx_Enable != false) && (DMA_tx_Enable == false))
  {
    DMAx_NVIC_Init(DMA1_Stream6_IRQn, 6, 0);
    USARTx_NVIC_Init(UART4_IRQn, 6, 0);
    USARTx_ReceiveToIdle_DMA(&uart4_485_handle, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
  }
  else if ((DMA_rx_Enable == false) && (DMA_tx_Enable != false))
  {
    DMAx_NVIC_Init(DMA1_Stream7_IRQn, 6, 0);
    USARTx_NVIC_Init(UART4_IRQn, 6, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&uart4_485_handle, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
  }
  else
  {
    USARTx_NVIC_Init(UART4_IRQn, 6, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&uart4_485_handle, uart4_rx_buf, UART4_485_RX_BUFFER_SIZE);
  }
}

void UART4_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uart4_485_handle);
}

void DMA1_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_uart4_rx_handle);
}

void DMA1_Stream7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_uart4_tx_handle);
}
