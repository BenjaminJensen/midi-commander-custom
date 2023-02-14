/*
 * buttons.c
 *
 *  Created on: 13 Feb 2023
 *      Author: ben
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_
#include <stdint.h>

void buttons_init(void);
uint16_t buttons_scan(void);
#endif
