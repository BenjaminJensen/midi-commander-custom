/*
 * menu_modifier.h
 *
 *  Created on: 25 Jul 2024
 *      Author: ben
 */

#ifndef INC_MENU_MODIFIER_H_
#define INC_MENU_MODIFIER_H_

#include <stdint.h>

void mm8_add1(uint8_t* val, uint8_t max);
void mm8_add10(uint8_t* val, uint8_t max);

void mm8_sub1(uint8_t* val, uint8_t min);
void mm8_sub10(uint8_t* val, uint8_t min);

#endif /* INC_MENU_MODIFIER_H_ */
