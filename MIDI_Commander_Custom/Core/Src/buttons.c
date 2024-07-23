/*
 * buttons.c
 *
 *  Created on: 13 Feb 2023
 *      Author: ben
 */

/****************************************
 * Includes
 ***************************************/
#include "buttons.h"
#include "event.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

/****************************************
 * Defines & Constants
 ***************************************/

// USB pins
#define USB_ID_Pin GPIO_PIN_12

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

enum button_state_e {BUTTON_PRESSED, BUTTON_RELEASED, BUTTON_HOLD};
typedef struct {
	uint8_t cnt;
	const GPIO_TypeDef* port;
	const uint16_t pin;
	enum button_state_e state;
	uint16_t hold_time;
} button_debounce_t;

#define NUM_BUT (10)

/****************************************
 * Private data
 ***************************************/
const int debounce_threshold = 5;
const uint16_t max_hold_time = 2000 / 10; // 2000 ms divided på 10 ms (scan time)

const int num_buttons = NUM_BUT;
static button_debounce_t buttons[NUM_BUT] = {
  {
    .port = GPIOA,
    .pin = SW_1_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
	},
  {
    .port = GPIOA,
    .pin = SW_2_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOB,
    .pin = SW_3_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOB,
    .pin = SW_4_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOB,
    .pin = SW_5_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOB,
    .pin = SW_A_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOC,
    .pin = SW_B_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOA,
    .pin = SW_C_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOA,
    .pin = SW_D_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  },
  {
    .port = GPIOA,
    .pin = SW_E_Pin,
    .cnt = 0,
    .state = BUTTON_RELEASED
  }
};

/****************************************
 * Private declarations
 ****************************************/
static void buttons_gpio_init(void);

/****************************************
 * Public functions
 ****************************************/
void buttons_init() {
	buttons_gpio_init();

	for(int i = 0; i < num_buttons; i++) {
		buttons[i].cnt = 0;
	}
}

/*
 * @brief Button scanning function
 *
 * Should be called every 10ms
 */
uint16_t buttons_scan() {
	uint16_t button_state = 0;
  event_t e;

  // Run through buttons
	for(int i = 0; i < num_buttons; i++) {
	  buttons[i].cnt = buttons[i].cnt << 1;
    if( (buttons[i].port->IDR & buttons[i].pin) == 0) {
      buttons[i].cnt += 0x01;
    }
    else {
      buttons[i].cnt &= 0xFE;
    }
    // Evaluate state (4 consecutive positive reads)
    if((buttons[i].cnt & 0x0F) == 0x0F) {
      if(buttons[i].hold_time < max_hold_time) {
        buttons[i].hold_time++;
        if(buttons[i].hold_time >= max_hold_time) {
          // Hold button event
          buttons[i].state = BUTTON_HOLD;
          e.event.type = EVENT_BUTTON_HOLD;
          e.event.data0 = i;
          event_put(e);
        }

      }
      // Button push
      if(buttons[i].state != BUTTON_PRESSED && buttons[i].state != BUTTON_HOLD) {
        buttons[i].state = BUTTON_PRESSED;
        e.event.type = EVENT_BUTTON_PRESS;
        e.event.data0 = i;
        event_put(e);
      }
      button_state |= (1 << i);
    }
    else if((buttons[i].cnt & 0xF0) == 0xF0 && buttons[i].cnt != 0xFF) {
      // release
      if(buttons[i].state != BUTTON_RELEASED) {
        buttons[i].state = BUTTON_RELEASED;
        // Reset the "hold" time keeper
        buttons[i].hold_time = 0;
        e.event.type = EVENT_BUTTON_RELEASE;
        e.event.data0 = i;
        event_put(e);
      }
    }
	}

	return button_state;
}

/****************************************
 * Private functions
 ***************************************/

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

  HAL_GPIO_WritePin(GPIOC, USB_ID_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins :  USB_ID_Pin */
  GPIO_InitStruct.Pin = USB_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
