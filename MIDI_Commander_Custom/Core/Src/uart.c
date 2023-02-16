/*
 * uart.c
 *
 *  Created on: 16 Feb 2023
 *      Author: ben
 */

#include "uart.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"


static void uart_hal_init(void);

/***************************************
 * Global variables :-(
 ***************************************/
UART_HandleTypeDef huart2;

/***************************************
 * Public functions
 ***************************************/
void uart_init(void) {

 uart_hal_init();
}

void uart_putc(uint8_t c) {
  HAL_UART_Transmit(&huart2, &c, 1, 10);
}
/***************************************
 * private functions
 ***************************************/

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void uart_hal_init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 31250;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}
