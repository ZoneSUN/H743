/** 
 * @file USART2_485.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #include "USART2_485.h"
 
 /* 扩展变量 ------------------------------------------------------------------*/
UART_HandleTypeDef usart2_485_handle;
DMA_HandleTypeDef dma_usart2_rx_handle;
DMA_HandleTypeDef dma_usart2_tx_handle;
/* 0x24000000 起是 AXI SRAM，DMA1 能写入。SRAM4(0x38000000) DMA1 访问不到。 */
uint8_t usart2_rx_buf[USART2_485_RX_BUFFER_SIZE] __attribute__((at(0x24000420), zero_init));


 /* 私有函数原形 --------------------------------------------------------------*/
void USART2_485_Start(uint32_t baudrate, bool DMA_tx_Enable, bool DMA_rx_Enable)
{

  // 内核时钟选 D2PCLK1。APB1 是 100MHz，不是 400MHz
  HAL_StatusTypeDef clk_status = USARTx_PeriphCLK_Config(USART2, RCC_USART234578CLKSOURCE_D2PCLK1);
  printf("USART2_485_Start status: %d, PCLK1=%luM\r\n",
         (int)clk_status, (unsigned long)HAL_RCC_GetPCLK1Freq()/1000000);

  //GPIO初始化：PD5 发送，PD6 接收，复用 USART2
  HAL_StatusTypeDef gpio_status = USARTx_GPIO_Init(GPIOD, GPIOD, GPIO_PIN_5, GPIO_PIN_6, GPIO_AF7_USART2, GPIO_AF7_USART2);

  //串口时钟使能
  USARTx_USART_CLK_Init(USART2); //USART2时钟使能

  //串口初始化
  /* 与调试串口、板载 RS485 例程一致：115200 8N1。9600 时对端按 115200 收会显示成没数据 */
  HAL_StatusTypeDef uart_status = USARTx_Init(&usart2_485_handle, USART2, baudrate);
  (void)gpio_status;
  (void)uart_status;

  if ((DMA_tx_Enable != false) || (DMA_rx_Enable != false))
  {
    DMAx_CLK_Init(DMA1);
  }

  if (DMA_rx_Enable != false)
  {
    HAL_StatusTypeDef dma_rx_status = USARTx_DMA_Init(&usart2_485_handle,
                                                      &dma_usart2_rx_handle,
                                                      DMA1_Stream0,
                                                      DMA_REQUEST_USART2_RX,
                                                      DMA_PERIPH_TO_MEMORY);
    printf("USARTx_DMA_Init RX status: %d\r\n", (int)dma_rx_status);
  }

  if (DMA_tx_Enable != false)
  {
    HAL_StatusTypeDef dma_tx_status = USARTx_DMA_Init(&usart2_485_handle,
                                                      &dma_usart2_tx_handle,
                                                      DMA1_Stream1,
                                                      DMA_REQUEST_USART2_TX,
                                                      DMA_MEMORY_TO_PERIPH);
    printf("USARTx_DMA_Init TX status: %d\r\n", (int)dma_tx_status);
  }

  if (DMA_rx_Enable != false && DMA_tx_Enable != false)//DMA_rx和DMA_tx都使能
  {
    DMAx_NVIC_Init(DMA1_Stream0_IRQn, 3, 0);
    DMAx_NVIC_Init(DMA1_Stream1_IRQn, 3, 0);
    USARTx_NVIC_Init(USART2_IRQn, 3, 0);
    USARTx_ReceiveToIdle_DMA(&usart2_485_handle, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
    printf("DMA_rx_Enable != false && DMA_tx_Enable != false\r\n");
  }
  else if (DMA_rx_Enable != false && DMA_tx_Enable == false)//DMA_rx使能，DMA_tx不使能
  {
    DMAx_NVIC_Init(DMA1_Stream0_IRQn, 3, 0);
    USARTx_NVIC_Init(USART2_IRQn, 3, 0);
    USARTx_ReceiveToIdle_DMA(&usart2_485_handle, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
    printf("DMA_rx_Enable != false && DMA_tx_Enable == false\r\n");
  }
  else if (DMA_rx_Enable == false && DMA_tx_Enable != false)//DMA_rx不使能，DMA_tx使能
  {
    DMAx_NVIC_Init(DMA1_Stream1_IRQn, 3, 0);
    USARTx_NVIC_Init(USART2_IRQn, 3, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&usart2_485_handle, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
    printf("DMA_rx_Enable == false && DMA_tx_Enable != false\r\n");
  }
  else//DMA_rx和DMA_tx都不使能
  {
    USARTx_NVIC_Init(USART2_IRQn, 3, 0);
    HAL_UARTEx_ReceiveToIdle_IT(&usart2_485_handle, usart2_rx_buf, USART2_485_RX_BUFFER_SIZE);
    printf("DMA_rx_Enable == false && DMA_tx_Enable == false\r\n");
  }
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&usart2_485_handle);
}

//DMA1_Stream0_IRQHandler 中断处理函数
void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_usart2_rx_handle);//DMA1_Stream0 中断处理函数
}

//DMA1_Stream1_IRQHandler 中断处理函数
void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&dma_usart2_tx_handle);//DMA1_Stream1 中断处理函数
}
