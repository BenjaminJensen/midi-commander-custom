/*
 * file_system.h
 *
 *  Created on: 16 Jul 2024
 *      Author: ben
 */

#ifndef FILE_SYSTEM_FILE_SYSTEM_H_
#define FILE_SYSTEM_FILE_SYSTEM_H_
#include <stdint.h>
#include "settings_types.h"

void file_system_init();
int file_system_load_settings(settings_t* settings);
int file_system_store_settings(settings_t* settings);
int file_system_load_preset(uint16_t nr, preset_t* preset);
int file_system_store(uint16_t nr, preset_t* preset);

#endif /* FILE_SYSTEM_FILE_SYSTEM_H_ */
