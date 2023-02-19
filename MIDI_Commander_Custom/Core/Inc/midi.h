/*
 * midi.h
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#ifndef INC_MIDI_H_
#define INC_MIDI_H_

#include <stdint.h>
void midi_send_cc(uint8_t chan, uint8_t cc, uint8_t val);

#endif /* INC_MIDI_H_ */
