/*
 * buttons.c
 *
 *  Created on: 13 Feb 2023
 *      Author: ben
 */

#include "buttons.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

// USB pins
#define USB_ID_Pin GPIO_PIN_12

// LED pins
#define LED_1_Pin GPIO_PIN_3
#define LED_2_Pin GPIO_PIN_8
#define LED_3_Pin GPIO_PIN_14
#define LED_4_Pin GPIO_PIN_12
#define LED_5_Pin GPIO_PIN_2
#define LED_A_Pin GPIO_PIN_4
#define LED_B_Pin GPIO_PIN_15
#define LED_C_Pin GPIO_PIN_13
#define LED_D_Pin GPIO_PIN_5
#define LED_E_Pin GPIO_PIN_1

// Switch pins
#define SW_1_Pin GPIO_PIN_15
#define SW_2_Pin GPIO_PIN_10
#define SW_3_Pin GPIO_PIN_15
#define SW_4_Pin GPIO_PIN_13
#define SW_5_Pin GPIO_PIN_10
#define SW_A_Pin GPIO_PIN_5
#define SW_B_Pin GPIO_PIN_14
#define SW_C_Pin GPIO_PIN_0
#define SW_D_Pin GPIO_PIN_4
#define SW_E_Pin GPIO_PIN_6

typedef struct {
	uint8_t cnt;
	const GPIO_TypeDef* port;
	const uint16_t pin;
} button_debounce_t;

#define NUM_BUT (10)
const int debounce_threshold = 5;

const int num_buttons = NUM_BUT;
static button_debounce_t buttons[NUM_BUT] = {
  {
    .port = GPIOA,
    .pin = SW_1_Pin,
    .cnt = 0
	},
  {
    .port = GPIOA,
    .pin = SW_2_Pin,
    .cnt = 0
  },
  {
    .port = GPIOB,
    .pin = SW_3_Pin,
    .cnt = 0
  },
  {
    .port = GPIOB,
    .pin = SW_4_Pin,
    .cnt = 0
  },
  {
    .port = GPIOB,
    .pin = SW_5_Pin,
    .cnt = 0
  },
  {
    .port = GPIOB,
    .pin = SW_A_Pin,
    .cnt = 0
  },
  {
    .port = GPIOC,
    .pin = SW_B_Pin,
    .cnt = 0
  },
  {
    .port = GPIOA,
    .pin = SW_C_Pin,
    .cnt = 0
  },
  {
    .port = GPIOA,
    .pin = SW_D_Pin,
    .cnt = 0
  },
  {
    .port = GPIOA,
    .pin = SW_E_Pin,
    .cnt = 0
  }
};
/*
 * Private declarations
 */
static void buttons_gpio_init(void);

/*
 * Public functions
 */
void buttons_init() {
	buttons_gpio_init();

	for(int i = 0; i < num_buttons; i++) {
		buttons[i].cnt = 0;
	}
}

uint16_t buttons_scan() {
	uint16_t button_state = 0;
/*
	uint32_t ida = GPIOA->IDR;
	uint32_t idb = GPIOB->IDR;
	uint32_t idc = GPIOC->IDR;
*/
	// Port A: SW_C_Pin | SW_D_Pin | SW_E_Pin | SW_2_Pin | SW_1_Pin
	// Port B: SW_5_Pin | SW_4_Pin | SW_3_Pin | SW_A_Pin
	// Port C: SW_B_Pin

	for(int i = 0; i < num_buttons; i++) {
	 if( (buttons[i].port->IDR & buttons[i].pin) == 0) {
	  button_state |= (1 << i);
	 }
	}
	/*
	if(ida & SW_1_Pin) {
		button_state |= (1 << 0);
	}
	if(ida & SW_2_Pin) {
		button_state |= (1 << 1);
	}
	if(idb & SW_3_Pin) {
		button_state |= (1 << 2);
	}
	if(idb & SW_4_Pin) {
		button_state |= (1 << 3);
	}
	if(idb & SW_5_Pin) {
		button_state |= (1 << 4);
	}
	if(idb & SW_A_Pin) {
		button_state |= (1 << 5);
	}
	if(idc & SW_B_Pin) {
		button_state |= (1 << 6);
	}
	if(ida & SW_C_Pin) {
		button_state |= (1 << 7);
	}
	if(ida & SW_D_Pin) {
		button_state |= (1 << 8);
	}
	if(ida & SW_E_Pin) {
		button_state |= (1 << 9);
	}
*/
	return button_state;
}
/*
 * Private functions
 */

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void buttons_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();


  HAL_GPIO_WritePin(GPIOC, LED_C_Pin|LED_B_Pin|USB_ID_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOA, LED_D_Pin|LED_2_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, LED_E_Pin|LED_5_Pin|LED_4_Pin|LED_3_Pin
                          |LED_1_Pin|LED_A_Pin, GPIO_PIN_RESET);


  /*Configure GPIO pins : LED_C_Pin LED_B_Pin USB_ID_Pin */
  GPIO_InitStruct.Pin = LED_C_Pin | LED_B_Pin | USB_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_B_Pin */

  /*
   * LED Outputs
   */
  GPIO_InitStruct.Pin = LED_D_Pin | LED_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_E_Pin | LED_5_Pin | LED_4_Pin | LED_3_Pin | LED_1_Pin | LED_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


  /*
   * Switch Inputs
   */
  // Port A
  GPIO_InitStruct.Pin = SW_C_Pin | SW_D_Pin | SW_E_Pin | SW_2_Pin | SW_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // Port B
  GPIO_InitStruct.Pin = SW_5_Pin | SW_4_Pin | SW_3_Pin | SW_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  // Port C
  GPIO_InitStruct.Pin = SW_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
