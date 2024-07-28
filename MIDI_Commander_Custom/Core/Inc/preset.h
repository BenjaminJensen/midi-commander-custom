/*
 * preset.h
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#ifndef INC_PRESET_H_
#define INC_PRESET_H_

#include <stdint.h>
#include "event.h"

void preset_init(void);
int preset_process_event(event_t e);

#endif /* INC_PRESET_H_ */
