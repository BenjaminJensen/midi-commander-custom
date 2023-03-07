/*
 * settings.c
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */


#include "settings.h"
#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"
#include <stdio.h>


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
 * Private function declarations
 ***************************************/
static int settings_load_preset(int nr);

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


/****************************************
 * Private function declarations
 ***************************************/
//0x5X
static int settings_load_preset(int nr) {
  //0x5X
  uint8_t data[8] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7 };
  uint8_t rec[8] = {0};

  I2C_HandleTypeDef hi2c1;
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100*1000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    SEGGER_RTT_WriteString(0, "Settings: Unable to initialize I2C!\n");
  }
  uint8_t ctrl_addr = 0xA0;

  for(int i = 0; i < 8; i++) {
    uint8_t ctrl = ctrl_addr | 0x0; // Write
    HAL_I2C_Mem_Write(&hi2c1, ctrl, 0x00, 1, data, 1, 10);
  }

  char buf[64];
  int size = 0;
  for(int i = 0; i < 8; i++) {
    uint8_t ctrl = ctrl_addr | 0x1; // Read
    HAL_I2C_Mem_Read(&hi2c1, ctrl, 0x00, 1, rec, 1, 10);
    size = sprintf(buf, "w:%x r:%x", data[i], rec[i]);
    buf[size] = 0;
    SEGGER_RTT_WriteString(0, buf);
  }
  return 0;
}
