/*
 * settings_factory.c
 *
 *  Created on: 16 Aug 2023
 *      Author: ben
 */

#include "settings_factory.h"

static const settings_t settings_factory = {
  .pcs = {
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 0,
       .name = "ML10X",
     },
     {
       .mode = PC_MODE_ON,
       .chan = 1,
       .pc = 1,
       .name = "H90",
     },
     {
       .mode = PC_MODE_ON,
       .chan = 2,
       .pc = 2,
       .name = "TC X3",
     },
     {
       .mode = PC_MODE_ON,
       .chan = 3,
       .pc = 3,
       .name = "Default PC3",
     },
     {
       .mode = PC_MODE_ON,
       .chan = 4,
       .pc = 4,
       .name = "Default PC4",
     },
  },
  .ias = {
      // IA 0-7
      // IA 0-9 formatted

/*************************************************
Page 1: IA 0-7
**************************************************/
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 10, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'S', 'P', 'E', 'C', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 11, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'S', 'p', ' ', 'C', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 12, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'E', 'P', ' ', 'B', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 13, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'D', 'U', 'A', 'L', 0},
      },

      /*  Moving arround in MX10L Loops due to D-TIP/RING inactivity*/
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 14, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'I', 'A', '0', '4', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 15, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'I', 'A', '0', '5', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 18, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'C', 'Y', 'O', 'N', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 19, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'L', 'I', 'G', 'H', 0},
      },
/*************************************************
      Page 2: IA 8-15
**************************************************/

      /* ML10X D-RING/TIP */
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 16, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'I', 'A', '0', '8', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 0,
        .midi_data0 = 17, // CC number
        .midi_data1 = 0xFF, // CC On value
        .midi_data2 = 0x00, // CC Off value
        .id = {'I', 'A', '0', '9', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 1,
        .midi_data0 = 10,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'H', '9', '0', 'A', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 1,
        .midi_data0 = 11,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'H', '9', '0', 'B', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 92,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '1', '2', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 2,
        .midi_data0 = 102,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'T', 'C', '1', ' ', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 2,
        .midi_data0 = 103,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'T', 'C', '2', ' ', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 2,
        .midi_data0 = 104,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'T', 'C', '3', ' ', 0},
      },
/*************************************************
      Page 3: IA 16-23
**************************************************/
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 96,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '6', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_TOGGLE,
       .midi_chan = 10,
       .midi_data0 = 97,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '7', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 98,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '8', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 99,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '9', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 100,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '0', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 101,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '1', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 102,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '2', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 103,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '3', 0},
      },
  }
};

const settings_t* get_settings_factory() {
  return &settings_factory;
}
