/*
 * settings.c
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */


#include "settings.h"
#define NUM_IA (3)

static ia_t ias[NUM_IA] = {
  {
    .type = IA_TYPE_CC,
    .mode = IA_MODE_LATCH,
    .state = IA_STATE_OFF,
    .midi_chan = 10,
    .midi_data0 = 80,
    .midi_data1 = 0x7F,
    .midi_data2 = 0,
  },
  {
    .type = IA_TYPE_CC,
    .mode = IA_MODE_TOGGLE,
    .state = IA_STATE_OFF,
    .midi_chan = 10,
    .midi_data0 = 81,
    .midi_data1 = 0x7F,
    .midi_data2 = 0,
  },
  {
    .type = IA_TYPE_CC,
    .mode = IA_MODE_LATCH,
    .state = IA_STATE_OFF,
    .midi_chan = 10,
    .midi_data0 = 82,
    .midi_data1 = 0x7F,
    .midi_data2 = 0,
  },
};

int settings_get_ia(uint8_t nr, ia_t **ia) {
  int error = 0;
  if(nr < NUM_IA) {
    *ia = &(ias[nr]);
  }
  else {
    ia = 0;
    error = -1;
  }
  return error;
}
