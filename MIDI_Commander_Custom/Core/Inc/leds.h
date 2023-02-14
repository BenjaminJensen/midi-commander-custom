/*
 * leds.h
 *
 *  Created on: Feb 14, 2023
 *      Author: ben
 */

#ifndef INC_LEDS_H_
#define INC_LEDS_H_

#include <stdint.h>
enum led_states_e {LED_ON, LED_OFF, LED_BLINK};

void leds_init(void);
void leds_set_led(uint8_t led, enum led_states_e state);
#endif /* INC_LEDS_H_ */
