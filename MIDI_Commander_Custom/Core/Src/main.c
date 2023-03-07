/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "display.h"
#include "stm32f1xx_hal_tim.h"

#include "buttons.h"
#include "leds.h"
#include "uart.h"
#include "app.h"
#include "SEGGER_RTT.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

  TIM_HandleTypeDef htim2;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
  static int settings_load_preset(int nr) {
    //0x5X
    uint8_t data[8] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7 };
    uint8_t rec[8] = {0};

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    I2C_HandleTypeDef hi2c_handle;
    hi2c_handle.Instance = I2C1;
    hi2c_handle.Init.ClockSpeed = 300*1000;
    hi2c_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c_handle.Init.OwnAddress1 = 0;
    hi2c_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c_handle.Init.OwnAddress2 = 0;
    hi2c_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c_handle) != HAL_OK)
    {
      SEGGER_RTT_WriteString(0, "Settings: Unable to initialize I2C!\n");
    }
    uint8_t ctrl_addr = 0xA0;

    for(int i = 0; i < 8; i++) {
      uint8_t ctrl = ctrl_addr | 0x0; // Write
      HAL_I2C_Mem_Write(&hi2c_handle, ctrl, i, 1, &data[i], 1, 10);
      HAL_Delay(5);
    }

    char buf[64];
    int size = 0;
    for(int i = 0; i < 8; i++) {
      uint8_t ctrl = ctrl_addr | 0x1; // Read
      HAL_I2C_Mem_Read(&hi2c_handle, ctrl, i, 1, &rec[i], 1, 10);
      size = sprintf(buf, "w:%x r:%x\n", data[i], rec[i]);
      buf[size] = 0;
      SEGGER_RTT_WriteString(0, buf);
    }
    return 0;
  }
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;

DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
uint8_t f_sys_config_complete = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
static void MX_TIM2_Init(TIM_HandleTypeDef *handler);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
static void send_midi(uint8_t but, uint8_t state) {
  uint8_t cc = 0;

  switch(but) {
    case 0:
      cc = 80;
      break;
    case 1:
      cc = 81;
      break;
    case 2:
      cc = 82;
      break;
    default:
      cc = but;
      break;
  }

  uart_putc(0xB0); // CC chan 0
  uart_putc(cc); // CC nr
  if(state == 0) {
    uart_putc(0x00); // value
  }
  else {
    uart_putc(0x7f); // value

  }

}
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	static uint16_t b_state = 0;
	// Get buttons
	b_state = buttons_scan();
	// Update leds
  led_update_task();

  /*
 static uint8_t cnt = 0;
  static uint16_t b_state_old = 0;
  char rtt_buf[10] = "But0,s0\r\n\0";

	if(b_state != b_state_old) {
	 for(int i = 0; i <10; i++) {
	   if((b_state & (1 << i)) != (b_state_old & (1 << i))) {
	     rtt_buf[3] = '0' + i;
	     if(b_state & (1 << i)) {
        leds_set_led(i, LED_ON);
        send_midi(i, 1);
        rtt_buf[6] = '1';
        SEGGER_RTT_WriteString(0, rtt_buf);
       }
       else {
        leds_set_led(i, LED_OFF);
        send_midi(i, 0);
        rtt_buf[6] = '0';
        SEGGER_RTT_WriteString(0, rtt_buf);
       }
	   }
	 } // for loop

   b_state_old = b_state;
	}

	// update display
	cnt++;
	if(cnt > 4) {
	 cnt = 0;
	 b_state_old = b_state;
		display_disp_button(b_state);
	}
  */
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

  /* USER CODE BEGIN Init */

   SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal - Bongo Controller\r\n");
   SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

settings_load_preset(0);
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  buttons_init();
  leds_init();
  uart_init();
  MX_TIM2_Init(&htim2);

  // Reset the USB interface in case it's still plugged in.
  HAL_GPIO_WritePin(USB_ID_GPIO_Port, USB_ID_Pin, GPIO_PIN_RESET);

  display_init(&hi2c1);

  // Check we've got a 256kB device, in case Melo switch to a smaller device at some point
  uint16_t flash_size = (*(uint16_t*)FLASHSIZE_BASE);
  uint16_t min_size = 256;
  if(flash_size < min_size) {
	  char msg[25];
	  sprintf(msg, "Mem %3dkb < %3dkb", flash_size, min_size);
	  Error(msg);
  }

  HAL_Delay(1000);
  HAL_GPIO_WritePin(USB_ID_GPIO_Port, USB_ID_Pin, GPIO_PIN_SET);


  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  app_init();
  while (1)
  {
	  //handle_switches();
    app_run();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_TIM2_Init(TIM_HandleTypeDef *handler)
{

	/* Peripheral clock enable */
	__HAL_RCC_TIM2_CLK_ENABLE();
	/* TIM2 interrupt Init */
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  // CLK = 72mhz
  // TIM_CLK = 72mhz
  // TIM_CLK/(TIM_PSC+1)/(TIM_ARR + 1)
  // 72mhz / ( 1001 - 1 ) / (721 - 1) = 10ms
  handler->Instance = TIM2;
  handler->Init.Prescaler = 1001;
  handler->Init.CounterMode = TIM_COUNTERMODE_UP;
  handler->Init.Period = 721;
  handler->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  handler->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(handler) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(handler, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(handler, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }


}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}


/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}


/* USER CODE BEGIN 4 */

/*
 * @brief This function can be called to display an error message on the screen,
 * if the display is on, before calling Error_Handler() to stop all operations.
 */

void Error(char *msg) {
/*
	if (ssd1306_GetDisplayOn() == 0) {
		// Display is on
		ssd1306_Fill(Black);
		ssd1306_SetCursor(2, 0);
		ssd1306_WriteString("Error:", Font_6x8, White);
		ssd1306_SetCursor(2, 9);
		ssd1306_WriteString(msg, Font_6x8, White);
		ssd1306_UpdateScreen();
	}
  */
	Error_Handler();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
