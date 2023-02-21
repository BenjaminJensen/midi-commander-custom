/*
 * preset.h
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#ifndef INC_PRESET_H_
#define INC_PRESET_H_

#include <stdint.h>

void preset_ia(uint8_t nr, uint8_t state);
void preset_bank_up(void);
void preset_bank_down(void);
void preset_load_relativ(uint8_t nr);
void preset_init(void);

#endif /* INC_PRESET_H_ */
