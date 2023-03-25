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
enum ia_type_e {IA_TYPE_DISABLED = 0, IA_TYPE_PC, IA_TYPE_CC};
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
  char id[5];
} ia_t;

// Preset
typedef struct __attribute__((packed)){
  uint16_t crc;
  uint8_t pc0;
  uint8_t pc1;
  uint8_t pc2;
  uint8_t pc3;
  uint8_t pc4;
  uint8_t ia0_7;
  uint8_t ia8_15;
  uint8_t ia16_23;
} preset_t;

enum pc_mode_e {PC_MODE_DISABLED, PC_MODE_ON};
typedef struct {
  uint8_t pc;
  enum pc_mode_e mode:4;
  uint8_t chan:4;
} pc_t;

/****************************************
 * Public functions
 ***************************************/
int settings_get_ia(uint8_t nr, ia_t const**ia);
void settings_init(void);
int settings_load_preset(int nr, preset_t* preset);
int settings_save_preset(int nr, preset_t* preset);

#endif /* INC_SETTINGS_H_ */
