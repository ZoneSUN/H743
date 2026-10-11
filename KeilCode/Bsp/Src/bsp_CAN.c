/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: MZXQ嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 
  * 功    能: FDCAN-双机通信
  ******************************************************************************
  * 说明：
  * 本例程配套MZXQstm32开发板使用。
  * 
  * 淘宝：
  * 论坛：MZXQ
  * 版权归MZXQ嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_CAN.h"
#include "bsp_debug_usart.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/

FDCAN_HandleTypeDef hFDCanx;
FDCAN_RxHeaderTypeDef FDCANx_Rx;
FDCAN_TxHeaderTypeDef FDCANx_Tx;

FDCAN_HandleTypeDef hFDCany;
FDCAN_RxHeaderTypeDef FDCANy_Rx;
FDCAN_TxHeaderTypeDef FDCANy_Tx;

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: CAN引脚初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void YS_CANx_MspInit(void)
{

  GPIO_InitTypeDef GPIO_InitStruct={0};

  /* 使能外设以及GPIO时钟 */
  CANx_CLK_ENABLE();
  CANx_GPIO_CLK_ENABLE();
  
  /**FDCAN1 GPIO Configuration    
  PI9     ------> FDCAN1_RX
  PB9     ------> FDCAN1_TX 
  */
  GPIO_InitStruct.Pin = CANx_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate=GPIO_AFx_CAN;
  HAL_GPIO_Init(CANx_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CANx_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate=GPIO_AFx_CAN;
  HAL_GPIO_Init(CANx_RX_GPIO_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(FDCANx_IT0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(FDCANx_IT0_IRQn);  
    
}

/**
  * 函数功能: CAN外设初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
/**
  * 函数功能: CAN外设初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void MX_CANx_Init(void)
{ 
  FDCAN_FilterTypeDef FDCAN_RXFilter={0};        
  /*CAN外设引脚初始化*/
  YS_CANx_MspInit();
  
  /*CAN单元初始化*/ 
  HAL_FDCAN_DeInit(&hFDCanx);                              //先清除以前的设置
  hFDCanx.Instance = FDCANx;
  hFDCanx.Init.FrameFormat = FDCAN_FRAME_FD_BRS;           //FD模式  
  hFDCanx.Init.Mode = FDCAN_MODE_NORMAL;                   //普通模式
  hFDCanx.Init.AutoRetransmission = DISABLE;               //关闭自动重传，传统模式下关闭
  hFDCanx.Init.TransmitPause = DISABLE;                    //关闭传输暂停    
  hFDCanx.Init.ProtocolException=DISABLE;                  //关闭协议异常处理
  hFDCanx.Init.NominalPrescaler = 2;                       //分频系数
  hFDCanx.Init.NominalSyncJumpWidth = 0x1;                 //重新同步跳跃宽度
  hFDCanx.Init.NominalTimeSeg1 = 0xF;                      //采样点之前标志时间段:2~256
  hFDCanx.Init.NominalTimeSeg2 = 0x4;                      //采样点之后标志时间段:2~128
  hFDCanx.Init.DataPrescaler = 2;                          //数据段预分频
  hFDCanx.Init.DataSyncJumpWidth = 0x4;                    //数据位同步跳跃宽度
  hFDCanx.Init.DataTimeSeg1 = 0xF;                         //采样点之前数据时间段
  hFDCanx.Init.DataTimeSeg2 = 0x4;                         //采样点之后数据时间段
  hFDCanx.Init.MessageRAMOffset = 0;                       //指定消息RAM起始地址
  hFDCanx.Init.StdFiltersNbr = 1;                          //标准信息ID滤波器编号
  hFDCanx.Init.ExtFiltersNbr = 0;                          //扩展信息ID滤波器编号
  hFDCanx.Init.RxFifo0ElmtsNbr = 1;                        //接收FIFO0元素编号
  hFDCanx.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;       //接收FIFO0元素大小：8字节
  hFDCanx.Init.RxBuffersNbr = 0;                           //接收缓冲编号
  hFDCanx.Init.TxEventsNbr = 0;                            //发送事件编号
  hFDCanx.Init.TxBuffersNbr = 0;                           //发送缓冲编号
  hFDCanx.Init.TxFifoQueueElmtsNbr = 1;                    //发送FIFO序列元素编号
  hFDCanx.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;  //发送FIFO序列模式
  hFDCanx.Init.TxElmtSize = FDCAN_DATA_BYTES_8;            //发送大小:8字节
  
  HAL_FDCAN_Init(&hFDCanx);

  /* 配置RX滤波器 */
  FDCAN_RXFilter.IdType=FDCAN_STANDARD_ID;                //标准ID
  FDCAN_RXFilter.FilterIndex=0;                           //滤波器索引                   
  FDCAN_RXFilter.FilterType=FDCAN_FILTER_MASK;            //滤波器类型
  FDCAN_RXFilter.FilterConfig=FDCAN_FILTER_TO_RXFIFO0;    //过滤器0关联到FIFO1  
  FDCAN_RXFilter.FilterID1=0x112;                         //32位ID1
  FDCAN_RXFilter.FilterID2=0x112;                         //32位ID2
  if(HAL_FDCAN_ConfigFilter(&hFDCanx,&FDCAN_RXFilter)!=HAL_OK)
  {
     printf("滤波器初始失败\n");
  }

  /* 配置接收 FIFO 0 watermark 为 2 */
  HAL_FDCAN_ConfigFifoWatermark(&hFDCanx, FDCAN_CFG_RX_FIFO0, 2);
  
  /* 开启FDCAN */
  HAL_FDCAN_Start(&hFDCanx);                               
  
  /* 使能FDCAN接收中断 */
  HAL_FDCAN_ActivateNotification(&hFDCanx,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0);
  	
}

/**
  * 函数功能: 发送一组数据
  * 输入参数: len:数据长度(最大为8)
              msg:数据指针,最大为8个字节
  * 返 回 值: 0,成功;其它，失败
  * 说    明: ID为0X12,标准帧,数据帧
  */
uint8_t FDCANx_Send_Msg(uint8_t* msg)
{	           
  FDCANx_Tx.Identifier=0x111;                             //32位ID
  FDCANx_Tx.IdType=FDCAN_STANDARD_ID;                     //标准ID
  FDCANx_Tx.TxFrameType=FDCAN_DATA_FRAME;                 //数据帧
  FDCANx_Tx.DataLength=FDCAN_DLC_BYTES_8;                 //数据长度
  FDCANx_Tx.ErrorStateIndicator=FDCAN_ESI_ACTIVE;             
  FDCANx_Tx.BitRateSwitch=FDCAN_BRS_ON;                  //关闭速率切换
  FDCANx_Tx.FDFormat=FDCAN_CLASSIC_CAN;                   //CAN模式                   
  FDCANx_Tx.TxEventFifoControl=FDCAN_NO_TX_EVENTS;       //无发送事件
  FDCANx_Tx.MessageMarker=0;    	
  /* 将需要发送的数据压入到TX FIFO */
  HAL_FDCAN_AddMessageToTxFifoQ(&hFDCanx,&FDCANx_Tx,msg);
	
  return 0;
}

/**
  * 函数功能: CAN引脚初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void YS_CANy_MspInit(void)
{

  GPIO_InitTypeDef GPIO_InitStruct={0};

  /* 使能外设以及GPIO时钟 */
  CANy_CLK_ENABLE();
  CANy_GPIO_CLK_ENABLE();
  
  /**FDCAN2 GPIO Configuration    
  PB5     ------> FDCAN2_RX
  PB6     ------> FDCAN2_TX 
  */
  GPIO_InitStruct.Pin = CANy_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate=GPIO_AFy_CAN;
  HAL_GPIO_Init(CANy_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CANy_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate=GPIO_AFy_CAN;
  HAL_GPIO_Init(CANy_RX_GPIO_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(FDCANy_IT0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(FDCANy_IT0_IRQn);  
    
}

/**
  * 函数功能: CAN外设初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void MX_CANy_Init(void)
{ 
  FDCAN_FilterTypeDef FDCAN_RXFilter={0};        
  /*CAN外设引脚初始化*/
  YS_CANy_MspInit();
  
  /*CAN单元初始化*/ 
  HAL_FDCAN_DeInit(&hFDCany);                              //先清除以前的设置
  hFDCany.Instance = FDCANy;
  hFDCany.Init.FrameFormat = FDCAN_FRAME_FD_BRS;           //FD模式  
  hFDCany.Init.Mode = FDCAN_MODE_NORMAL;                   //普通模式
  hFDCany.Init.AutoRetransmission = DISABLE;               //关闭自动重传，传统模式下关闭
  hFDCany.Init.TransmitPause = DISABLE;                    //关闭传输暂停    
  hFDCany.Init.ProtocolException=DISABLE;                  //关闭协议异常处理
  hFDCany.Init.NominalPrescaler = 2;                       //分频系数
  hFDCany.Init.NominalSyncJumpWidth = 0x1;                 //重新同步跳跃宽度
  hFDCany.Init.NominalTimeSeg1 = 0xF;                      //采样点之前标志时间段:2~256
  hFDCany.Init.NominalTimeSeg2 = 0x4;                      //采样点之后标志时间段:2~128
  hFDCany.Init.DataPrescaler = 2;                          //数据段预分频
  hFDCany.Init.DataSyncJumpWidth = 0x4;                    //数据位同步跳跃宽度
  hFDCany.Init.DataTimeSeg1 = 0xF;                         //采样点之前数据时间段
  hFDCany.Init.DataTimeSeg2 = 0x4;                         //采样点之后数据时间段
  hFDCany.Init.MessageRAMOffset = 0;                       //指定消息RAM起始地址
  hFDCany.Init.StdFiltersNbr = 1;                          //标准信息ID滤波器编号
  hFDCany.Init.ExtFiltersNbr = 0;                          //扩展信息ID滤波器编号
  hFDCany.Init.RxFifo0ElmtsNbr = 1;                        //接收FIFO0元素编号
  hFDCany.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;       //接收FIFO0元素大小：8字节
  hFDCany.Init.RxBuffersNbr = 0;                           //接收缓冲编号
  hFDCany.Init.TxEventsNbr = 0;                            //发送事件编号
  hFDCany.Init.TxBuffersNbr = 0;                           //发送缓冲编号
  hFDCany.Init.TxFifoQueueElmtsNbr = 1;                    //发送FIFO序列元素编号
  hFDCany.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;  //发送FIFO序列模式
  hFDCany.Init.TxElmtSize = FDCAN_DATA_BYTES_8;            //发送大小:8字节
  
  HAL_FDCAN_Init(&hFDCany);

  /* 配置RX滤波器 */
  FDCAN_RXFilter.IdType=FDCAN_STANDARD_ID;                //标准ID
  FDCAN_RXFilter.FilterIndex=0;                           //滤波器索引                   
  FDCAN_RXFilter.FilterType=FDCAN_FILTER_MASK;             //滤波器类型
  FDCAN_RXFilter.FilterConfig=FDCAN_FILTER_TO_RXFIFO0;    //过滤器0关联到FIFO1  
  FDCAN_RXFilter.FilterID1=0x111;                         //32位ID1
  FDCAN_RXFilter.FilterID2=0x111;                         //32位ID2
  if(HAL_FDCAN_ConfigFilter(&hFDCany,&FDCAN_RXFilter)!=HAL_OK)
  {
     printf("滤波器初始失败\n");
  }

  /* 配置接收 FIFO 0 watermark 为 2 */
  HAL_FDCAN_ConfigFifoWatermark(&hFDCany, FDCAN_CFG_RX_FIFO0, 2);
  
  /* 开启FDCAN */
  HAL_FDCAN_Start(&hFDCany);                               
  /* 使能FDCAN接收中断 */
  HAL_FDCAN_ActivateNotification(&hFDCany,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0);
  
}




/**
  * 函数功能: 发送一组数据
  * 输入参数: len:数据长度(最大为8)
              msg:数据指针,最大为8个字节
  * 返 回 值: 0,成功;其它，失败
  * 说    明: ID为0X12,标准帧,数据帧
  */
uint8_t FDCANy_Send_Msg(uint8_t* msg)
{	                   
  FDCANy_Tx.Identifier=0x112;                             //32位ID
  FDCANy_Tx.IdType=FDCAN_STANDARD_ID;                     //标准ID
  FDCANy_Tx.TxFrameType=FDCAN_DATA_FRAME;                 //数据帧
  FDCANy_Tx.DataLength=FDCAN_DLC_BYTES_8;                 //数据长度
  FDCANy_Tx.ErrorStateIndicator=FDCAN_ESI_ACTIVE;             
  FDCANy_Tx.BitRateSwitch=FDCAN_BRS_ON;                  //关闭速率切换
  FDCANy_Tx.FDFormat=FDCAN_CLASSIC_CAN;                   //CAN模式                   
  FDCANy_Tx.TxEventFifoControl=FDCAN_NO_TX_EVENTS;       //无发送事件
  FDCANy_Tx.MessageMarker=0;   	
  /* 将需要发送的数据压入到TX FIFO */
  HAL_FDCAN_AddMessageToTxFifoQ(&hFDCany,&FDCANy_Tx,msg);
	
  return 0;
}

/********** (C) COPYRIGHT 2019-2030 MZXQ嵌入式开发团队 *******END OF FILE******/
