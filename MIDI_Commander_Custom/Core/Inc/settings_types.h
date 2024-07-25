/*
 * settings_types.h
 *
 *  Created on: 26 Aug 2023
 *      Author: ben
 */

#ifndef INC_SETTINGS_TYPES_H_
#define INC_SETTINGS_TYPES_H_

#include <stdint.h>

#define NUM_IA (24)
#define NUM_PC (5)

// IA
enum ia_mode_e {IA_MODE_LATCH, IA_MODE_TOGGLE};
enum ia_state_e {IA_STATE_ON = 1, IA_STATE_OFF = 0};
enum ia_type_e {IA_TYPE_DISABLED = 0, IA_TYPE_PC, IA_TYPE_CC};
/*
 * WARNING: Will be written to the flash system
 * If changed, flash content will be invalid
 */
/*
 * type CC / PC
 * mode 4[latch / toggle] 4[state]
 * midi chan
 * midi0 (PC / CC)
 * midi1 (CC ON) (PC nr)
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
  char id[5];
} ia_t;

/*
 * WARNING: Will be written to the flash system
 * If changed, flash content will be invalid
 */
// Preset
typedef struct __attribute__((packed)){
  uint16_t crc;
  uint8_t pc[5];
  uint8_t ia0_7;
  uint8_t ia8_15;
  uint8_t ia16_23;
} preset_t;

enum pc_mode_e {PC_MODE_DISABLED, PC_MODE_ON};
/*
 * WARNING: Will be written to the flash system
 * If changed, flash content will be invalid
 */
typedef struct {
  uint8_t pc;
  enum pc_mode_e mode:4;
  uint8_t chan:4;
} pc_t;

/*
 * WARNING: Will be written to the flash system
 * If changed, flash content will be invalid
 */
typedef struct __attribute__((packed)){
  ia_t ias[NUM_IA];
  pc_t pcs[NUM_PC];
} settings_t ;

#endif /* INC_SETTINGS_TYPES_H_ */
