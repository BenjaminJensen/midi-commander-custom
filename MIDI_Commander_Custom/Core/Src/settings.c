/*
 * settings.c
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */


#include "settings.h"
#define NUM_IA (24)
#define NUM_PC (5)

typedef struct {
  ia_t ias[NUM_IA];
  pc_t pcs[NUM_PC];
} settings_t;

/****************************************
 * Factory Settings
 ***************************************/
 static const settings_t settings_factory = {
  .pcs = {
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 0,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 1,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 2,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 3,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 4,
     },
  },
  .ias = {
      // IA 0-7
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 80,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_TOGGLE,
        .midi_chan = 10,
        .midi_data0 = 81,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 82,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 83,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 84,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 85,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 86,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 87,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      // IA 8-15
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 88,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_TOGGLE,
        .midi_chan = 10,
        .midi_data0 = 89,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 90,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 91,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 92,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 93,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 94,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 95,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
      },
      // IA 8-15
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 96,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_TOGGLE,
       .midi_chan = 10,
       .midi_data0 = 97,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 98,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 99,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 100,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 101,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 102,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 103,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
      },
  }
};

 /****************************************
  * Local variables
  ***************************************/

static settings_t const *settings_current = 0;

/****************************************
 * Public functions
 ***************************************/
void settings_init() {
  // Load stores settings
  // validate loaded settings
  // if not valid
  //    Load factory settings
  settings_current = &settings_factory;
}

int settings_get_ia(uint8_t nr, ia_t const**ia) {
  int error = 0;
  if(nr < NUM_IA) {
    *ia = &(settings_current->ias[nr]);
  }
  else {
    ia = 0;
    error = -1;
  }
  return error;
}
