/*
 * preset.c
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#include "preset.h"
#include "midi.h"
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

// IA
enum ia_mode_e {IA_MODE_LATCH, IA_MODE_TOGGLE};
enum ia_state_e {IA_STATE_ON, IA_STATE_OFF};
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

#define NUM_IA (3)

/****************************************
 * Private functions
 ***************************************/
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

/****************************************
 * Private functions prototypes
 ***************************************/

static void ia_on (uint8_t nr);
static void ia_off (uint8_t nr);

/****************************************
 * Public functions
 ***************************************/
/*
 *
 */
void preset_ia(uint8_t nr, uint8_t state) {
  if(nr < NUM_IA) {
    ia_t ia = ias[nr];
    if(state == 1) {
      // State on
      if(ia.mode == IA_MODE_LATCH) {
        if(ia.state == IA_STATE_OFF) {
          ia_on(nr);
        }
        else {
          ia_off(nr);
        }
      }
      else if(ia.mode == IA_MODE_TOGGLE) {
        ia_on(nr);
      }
      //
    }
    else {
      // State of
      if(ia.mode == IA_MODE_TOGGLE) {
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
static void ia_on (uint8_t nr) {

  ias[nr].state = IA_STATE_ON;

  if(ias[nr].type == IA_TYPE_CC) {
      midi_send_cc(ias[nr].midi_chan, ias[nr].midi_data0, ias[nr].midi_data1);
    }


  // Debug
  char buf[16];
  int num;

  num = sprintf(buf, "IA on(%d)\r\n", nr);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}

static void ia_off (uint8_t nr) {

  ias[nr].state = IA_STATE_OFF;
  if(ias[nr].type == IA_TYPE_CC) {
    midi_send_cc(ias[nr].midi_chan, ias[nr].midi_data0, ias[nr].midi_data2);
  }
  // Debug
  char buf[16];
  int num;

  num = sprintf(buf, "IA off(%d)\r\n", nr);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}
