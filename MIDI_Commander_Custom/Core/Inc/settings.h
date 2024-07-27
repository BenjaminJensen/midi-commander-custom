/*
 * settings.h
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include "settings_types.h"

/****************************************
 * Public functions
 ***************************************/
int settings_get_ia(uint8_t nr, ia_t const** ia);
int settings_get_pc(uint8_t nr, pc_t const** pc);
void settings_init(void);
int settings_load_preset(int nr, preset_t* preset);
int settings_save_preset(int nr, preset_t* preset);

#endif /* INC_SETTINGS_H_ */
