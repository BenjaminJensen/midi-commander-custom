/*
 * preset.c
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#include "preset.h"
#include "midi.h"
#include "settings.h"
#include <stdio.h>
#include "SEGGER_RTT.h"

/****************************************
 * Private types and variables
 ***************************************/

// Preset
typedef struct {
  uint8_t pc0;
  uint8_t pc1;
  uint8_t pc2;
  uint8_t pc3;
  uint8_t pc4;
  uint8_t ia0_7;
  uint8_t ia8_15;
  uint8_t ia16_23;
} preset_t;

static const uint8_t preset_pr_bank = 4;
static const uint8_t preset_bank_max = 32; // 32 banks of 4 presets, total 128 presets
static preset_t preset_current = {0};
static uint8_t preset_number_current = 0;
static uint8_t preset_bank_current = 0;



/****************************************
 * Private functions
 ***************************************/


/****************************************
 * Private functions prototypes
 ***************************************/

static void ia_on (uint8_t nr);
static void ia_off (uint8_t nr);
static uint8_t ia_get_state(uint8_t nr);

/****************************************
 * Public functions
 ***************************************/
/*
 *
 */
void preset_ia(uint8_t nr, uint8_t state) {
  ia_t *ia = 0;
  settings_get_ia(nr, &ia);
  if(ia != 0) {
    if(state == 1) {
      // State on
      if(ia->mode == IA_MODE_LATCH) {
        if(ia_get_state(nr) == 0) {
          ia_on(nr);
        }
        else {
          ia_off(nr);
        }
      }
      else if(ia->mode == IA_MODE_TOGGLE) {
        ia_on(nr);
      }
      //
    }
    else {
      // State of
      if(ia->mode == IA_MODE_TOGGLE) {
        ia_off(nr);
      }
    }
  }
}

/*
 *  @brief Increases "bank" with one
 */
void preset_bank_up() {
  if(preset_bank_current == (preset_bank_max - 1)) {
    preset_bank_current = 0;
  }
  else {
    preset_bank_current++;
  }

  // Debug
  char buf[16];
  int num;

  num = sprintf(buf, "Bank up(%d)\r\n", preset_bank_current);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}

/*
 * @brief Decreases "bank" with one
 */
void preset_bank_down() {
  if(preset_bank_current == 0) {
    preset_bank_current = preset_bank_max - 1;
  }
  else {
    preset_bank_current--;
  }

  // Debug
  char buf[16];
  int num;

  num = sprintf(buf, "Bank down (%d)\r\n", preset_bank_current);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}

/*
 * @brief Loads preset relative to current "bank"
 */
void preset_load_relativ(uint8_t nr) {
  uint8_t new_preset = 0;
  if(nr < preset_pr_bank) {
    new_preset = (preset_bank_current * preset_pr_bank) + nr;
  }
  // Debug
  char buf[20];
  int num;

  num = sprintf(buf, "Load preset(%d)\r\n", new_preset);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}

/****************************************
 * Private functions
 ***************************************/
static uint8_t ia_get_state(uint8_t nr) {
  uint8_t state = 0;
  if( nr < 8 ) {
      state = (preset_current.ia0_7 & (1<<nr)) != 0 ? 1:0;
    }
    else if( nr < 16 ) {
      state = (preset_current.ia8_15 & (1<< (nr - 8 ))) != 0 ? 1:0;
    }
    else if( nr < 24 ) {
      state = (preset_current.ia16_23 & (1 << (nr - 16))) != 0 ? 1:0;
    }
  return state;
}

static void ia_on (uint8_t nr) {
  if( nr < 8 ) {
    preset_current.ia0_7 |= (1<<nr);
  }
  else if( nr < 16 ) {
    preset_current.ia8_15 |= ( 1 << (nr - 8) );
  }
  else if( nr < 24 ) {
    preset_current.ia16_23 |= ( 1 << (nr - 16) );
  }
  ia_t *ia = 0;
  settings_get_ia(nr, &ia);
  if(ia != 0) {
    if(ia->type == IA_TYPE_CC) {
        midi_send_cc(ia->midi_chan, ia->midi_data0, ia->midi_data1);
      }
  }

  // Debug
  char buf[16];
  int num;

  num = sprintf(buf, "IA on(%d)\r\n", nr);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}

static void ia_off (uint8_t nr) {

  if( nr < 8 ) {
    preset_current.ia0_7 &= ~(1<<nr);
  }
  else if( nr < 16 ) {
    preset_current.ia8_15 &= ~( 1 << (nr - 8) );
  }
  else if( nr < 24 ) {
    preset_current.ia16_23 &= ~( 1 << (nr - 16) );
  }
  ia_t *ia = 0;
  settings_get_ia(nr, &ia);
  if(ia != 0) {
    if(ia->type == IA_TYPE_CC) {
      midi_send_cc(ia->midi_chan, ia->midi_data0, ia->midi_data2);
    }
  }

  // Debug
  char buf[16];
  int num;

  num = sprintf(buf, "IA off(%d)\r\n", nr);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}
