/*
 * leds.c
 *
 *  Created on: Feb 14, 2023
 *      Author: ben
 */

#include "leds.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

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

static void leds_gpio_init(void);

typedef struct {
 enum led_states_e state;
 const GPIO_TypeDef* port;
 const uint16_t pin;
}led_t;

#define NUM_LEDS (10)

static const int number_leds = NUM_LEDS;

static led_t leds[NUM_LEDS] = {
  {
    .state = LED_OFF,
    .port = GPIOB,
    .pin = LED_1_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOA,
    .pin = LED_2_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOB,
    .pin = LED_3_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOB,
    .pin = LED_4_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOB,
    .pin = LED_5_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOB,
    .pin = LED_A_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOC,
    .pin = LED_B_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOC,
    .pin = LED_C_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOA,
    .pin = LED_D_Pin
  },
  {
    .state = LED_OFF,
    .port = GPIOB,
    .pin = LED_E_Pin
  },
};

void leds_init() {
 leds_gpio_init();
 for(int i = 0; i < number_leds; i++) {
  leds_set_led(i, LED_OFF);
 }
}

void leds_set_led(uint8_t led, enum led_states_e state) {
 //HAL_GPIO_WritePin(GPIOC, USB_ID_Pin, GPIO_PIN_RESET);
 if(led < number_leds) {
  switch(state) {
   case LED_OFF:
    HAL_GPIO_WritePin(leds[led].port, leds[led].pin, GPIO_PIN_SET);
    break;
   case LED_ON:
    HAL_GPIO_WritePin(leds[led].port, leds[led].pin, GPIO_PIN_RESET);
    break;
   case LED_BLINK:
    break;
   default:
    break;
  }

 }
}

static void leds_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

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

  GPIO_InitStruct.Pin = LED_B_Pin | LED_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


}
