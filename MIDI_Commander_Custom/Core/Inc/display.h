/*
 * display.h
 *
 *  Created on: 8 Jul 2021
 *      Author: D Harvie
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_
#include <stdint.h>

typedef struct {
  uint8_t pc[5];
  uint8_t bank;
} disp_preset_t;

void display_init(I2C_HandleTypeDef *);
void display_show_preset(disp_preset_t *preset);
void display_update(void);
#endif /* INC_DISPLAY_H_ */
