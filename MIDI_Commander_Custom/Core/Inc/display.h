/*
 * display.h
 *
 *  Created on: 8 Jul 2021
 *      Author: D Harvie
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_
#include <stdint.h>
#include "stm32f1xx_hal.h"

enum disp_leds_e {DISP_LED_P0 = 0x01, DISP_LED_P1 = 0x02, DISP_LED_P2 = 0x04, DISP_LED_P3 = 0x08, DISP_LED_IA0 = 0x10, DISP_LED_IA1 = 0x20, DISP_LED_IA2 = 0x40};
typedef struct {
  uint8_t pc[5];
  uint8_t bank;
  uint8_t leds;
} disp_preset_t;

typedef struct {
  uint8_t state;
  const char *id;
} disp_ia_t;

typedef struct {
  disp_ia_t ias[8];
  const char *name;
  uint8_t leds;
} disp_iax_t;

void display_init(I2C_HandleTypeDef *);
void display_show_preset(disp_preset_t *preset);
void display_update(void);
void display_bank_display(int bank);
void display_iax_display(disp_iax_t *data);

#endif /* INC_DISPLAY_H_ */
