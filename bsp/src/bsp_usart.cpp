#include "bsp_usart.hpp"

void USART_Init(void)
{
  USART1_Init();
  USART6_Init();
}

uint8_t receiveData6[50];
uint8_t receiveData1[50];

void USART6_Init()
{
  HAL_UARTEx_ReceiveToIdle_DMA(&huart6, receiveData6, sizeof(receiveData6));
  __HAL_DMA_DISABLE_IT(&hdma_ustart1_rx, DMA_IT_HT);
};
  // TODO: 配置 USART6 的 DMA 接收、发送及空闲中断，并启动接收。

void USART1_Init()
{
   HAL_UARTEx_ReceiveToIdle_DMA(&huart1, receiveData1, sizeof(receiveData1));
   __HAL_DMA_DISABLE_IT(&hdma_ustart1_rx, DMA_IT_HT);
};
  // TODO: 配置 USART1 的 DMA 接收、发送及空闲中断，并启动接收。
  

viod HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
{
  if(haurt == huart1){
   //要处理数据吗

   HAL_UARTEx_ReceiveToIdle_DMA(&huart1, receiveData1, sizeof(receiveData1));
   __HAL_DMA_DISABLE_IT(&hdma_ustart1_rx, DMA_IT_HT);
  };
  else if(huart == huart6){
   //处理数据
   
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, receiveData1, sizeof(receiveData1)); 
    __HAL_DMA_DISABLE_IT(&hdma_ustart6_rx, DMA_IT_HT);
  }
}

void USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, enum USART_Mode mode)
{
  // TODO: 检查参数，并按 mode 选择阻塞、DMA 或中断发送。
  if (huart == NULL || pData == NULL || Size == 0){
    return;
  }
  switch (mode)
  {
  case USART_MODE_IT:
    HAL_URAT_Transmit_IT(huart, pData, Size);
    break;
  case USART_MODE_DMA:
    HAL_UART_Transmit_DMA(huart, pData, size);
    break;
  default:
    HAL_UART_Transmit(huart, pData， Size)；
    break;
  }
}

void USART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  // TODO: 选择接收方式并启动 HAL UART 接收。

    // 1. 安全检查
    if (huart == NULL || pData == NULL || Size == 0) {
        return;
    }

    // 2. 启动接收
    // 注意：如果你已经在中断/回调里开启了 DMA 空闲接收，通常不需要再调用这个函数去抢接收权。
    // 这个函数通常用于：等待接收固定长度的数据。这里我们使用阻塞接收作为演示。
    
    // 阻塞接收：最多等待 100ms，直到收到 Size 个字节或者超时
    HAL_UART_Receive(huart, pData, Size, 100); 
    
    // 如果你希望使用中断方式接收（接收完成后触发 HAL_UART_RxCpltCallback），可以换成：
    // HAL_UART_Receive_IT(huart, pData, Size);
}

