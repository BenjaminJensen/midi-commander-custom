/*
 * settings.h
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include <stdint.h>
// IA
enum ia_mode_e {IA_MODE_LATCH, IA_MODE_TOGGLE};
enum ia_state_e {IA_STATE_ON = 1, IA_STATE_OFF = 0};
enum ia_type_e {IA_TYPE_PC, IA_TYPE_CC};
/*
 * type CC / PC
 * mode 4[latch / toggle] 4[state]
 * midi chan
 * midi0 (PC / CC)
 * midi1 (CC ON)
 * midi2 (CC OFF)
 */
typedef struct {
  enum ia_type_e type;
  uint8_t mode : 4;
  uint8_t state : 4;
  uint8_t midi_chan;
  uint8_t midi_data0;
  uint8_t midi_data1;
  uint8_t midi_data2;
} ia_t;


int settings_get_ia(uint8_t nr, ia_t **ia);

#endif /* INC_SETTINGS_H_ */
