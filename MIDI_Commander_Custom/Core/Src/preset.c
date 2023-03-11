/*
 * preset.c
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

/****************************************
 * Includes
 ***************************************/
#include "preset.h"
#include "midi.h"
#include "settings.h"
#include <stdio.h>
#include "SEGGER_RTT.h"
#include "display.h"
#include "event.h"

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
static uint8_t preset_bank_next = 0;

enum preset_state_e {PS_PRESET, PS_IA0, PS_IA1, PS_IA2, PS_BANK};
static enum preset_state_e preset_state = PS_PRESET;

/****************************************
 * Private functions prototypes
 ***************************************/

static void ia_on (uint8_t nr);
static void ia_off (uint8_t nr);
static uint8_t ia_get_state(uint8_t nr);
static int preset_process_event(event_t e);
static void preset_handle_preset(event_t e);

static void preset_ia(uint8_t nr, uint8_t state);
static void preset_bank_up(void);
static void preset_bank_down(void);
static void preset_load_relativ(uint8_t nr);
static void preset_update_display(void);
static void preset_bank_display(int bank);
static void preset_handle_bank(event_t e);
static void preset_page_next(void);
static void preset_handle_ia(event_t e, uint8_t offset);

/****************************************
 * Public functions
 ***************************************/
/*
 * @brief Initializes the preset module and registers all event handlers
 */
void preset_init() {
  settings_init();
  event_handler_register(EVENT_BUTTON_PRESS, &preset_process_event);
  event_handler_register(EVENT_BUTTON_RELEASE, &preset_process_event);
}

/****************************************
 * Private functions
 ***************************************/
/*
 *
 */
static void preset_update_display() {
  disp_preset_t p;
  p.bank = preset_bank_current;
  p.pc[0] = 2;
  p.pc[1] = 77;
  p.pc[2] = 88;
  p.pc[3] = 99;
  p.pc[4] = 101;
  display_show_preset(&p);
}
static void preset_bank_display(int bank) {
  display_bank_display(bank);
}
/*
 * @brief Event handler for all button events
 */
static int preset_process_event(event_t e) {

  // Handle button presses
  switch(preset_state) {
    case PS_PRESET:
      preset_handle_preset(e);
      break;
    case PS_IA0:
      preset_handle_ia(e, 0);
      break;
    case PS_IA1:
      preset_handle_ia(e, 1);
      break;
    case PS_IA2:
      preset_handle_ia(e, 2);
      break;
    case PS_BANK:
      preset_handle_bank(e);
      break;
    default:
      break;
  }

  disp_iax_t data;

  ia_t const*ia = 0;
  // Show resulting state
  switch(preset_state) {
     case PS_PRESET:
       preset_update_display();
       break;
     case PS_IA0:
       for(int i = 0 ; i < 8; i++) {
         settings_get_ia(i, &ia);
         data.ias[i].id = &(ia->id);
       }
       data.name = "IA 1-8";
       display_iax_display(&data);
       break;
     case PS_IA1:
       for(int i = 0 ; i < 8; i++) {
         settings_get_ia(i + 8, &ia);
         data.ias[i].id = &(ia->id);
       }
       data.name = "IA 9-16";
       display_iax_display(&data);
       break;
     case PS_IA2:
       for(int i = 0 ; i < 8; i++) {
         settings_get_ia(i + 16, &ia);
         data.ias[i].id = &(ia->id);
       }
       data.name = "IA 17-24";
       display_iax_display(&data);
       break;
     case PS_BANK:
       preset_bank_display(preset_bank_next);
       break;
     default:
       break;
   }

  return 0;
}
/*
 * @brief Button handler for state "preset"
 */
static void preset_handle_preset(event_t e) {
  switch(e.event.data0) {
    case 0:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_page_next();
      }
      break;
    case 1:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(0, 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(0, 0);
      }
      break;
    case 2:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(1, 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(1, 0);
      }
      break;
    case 3:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(2, 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(2, 0);
      }
      break;
    case 4:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_bank_up();
      }
      break;
    case 5:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(0);
      }
      break;
    case 6:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(1);
      }
      break;
    case 7:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(2);
      }
      break;
    case 8:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(3);
      }
      break;
    case 9:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_bank_down();
      }
      break;
    default:
      break;
  }
}
/*
 * @brief Button handler for "bank" state
 */
static void preset_handle_bank(event_t e) {
  switch(e.event.data0) {
    case 4:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_bank_up();
      }
      break;
    case 5:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(0);
      }
      break;
    case 6:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(1);
      }
      break;
    case 7:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(2);
      }
      break;
    case 8:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_load_relativ(3);
      }
      break;
    case 9:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_bank_down();
      }
      break;
    default:
      break;
  }
}
/*
 * @brief Button handler for state "IAx"
 */
static void preset_handle_ia(event_t e, uint8_t offset) {
  switch(e.event.data0) {
    case 0:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_page_next();
      }
      break;
    case 1:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(0 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(0 + (8 * offset), 0);
      }
      break;
    case 2:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(1 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(1 + (8 * offset), 0);
      }
      break;
    case 3:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(2 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(2 + (8 * offset), 0);
      }
      break;
    case 4:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(3 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(3 + (8 * offset), 0);
      }
      break;
    case 5:
      // Save?
      break;
    case 6:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(4 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(4 + (8 * offset), 0);
      }
      break;
    case 7:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(5 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(5 + (8 * offset), 0);
      }
      break;
    case 8:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(6 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(6 + (8 * offset), 0);
      }
      break;
    case 9:
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_ia(7 + (8 * offset), 1);
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_ia(7 + (8 * offset), 0);
      }
      break;
    default:
      break;
  }
}
/*
 *
 */
static void preset_page_next() {
  switch(preset_state) {
    case PS_PRESET:
      preset_state = PS_IA0;
      break;
    case PS_IA0:
      preset_state = PS_IA1;
      break;
    case PS_IA1:
      preset_state = PS_IA2;
      break;
    case PS_IA2:
      preset_state = PS_PRESET;
      break;
    default:
      break;
  }
  char buf[16];
  int num;
  num = sprintf(buf, "Next Page:%d\n", preset_state);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}
/*
 * @brief Handle IA button press
 */
static void preset_ia(uint8_t nr, uint8_t state) {
  ia_t const*ia = 0;
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
static void preset_bank_up() {
  if(preset_bank_next == (preset_bank_max - 1)) {
    preset_bank_next = 0;
  }
  else {
    preset_bank_next++;
  }

  if(preset_bank_current == preset_bank_next) {
    preset_state = PS_PRESET;
  }
  else {
    preset_state = PS_BANK;
  }

  // Debug
  char buf[32];
  int num;

  num = sprintf(buf, "Bank up c:%d n:%d\r\n", preset_bank_current, preset_bank_next);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}

/*
 * @brief Decreases "bank" with one
 */
static void preset_bank_down() {
  if(preset_bank_next == 0) {
    preset_bank_next = preset_bank_max - 1;
  }
  else {
    preset_bank_next--;
  }

  if(preset_bank_current == preset_bank_next) {
    preset_state = PS_PRESET;
  }
  else {
    preset_state = PS_BANK;
  }

  // Debug
  char buf[32];
  int num;

  num = sprintf(buf, "Bank down c:%d n:%d\r\n", preset_bank_current, preset_bank_next);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}

/*
 * @brief Loads preset relative to current "bank"
 */
static void preset_load_relativ(uint8_t nr) {
  uint8_t new_preset = 0;
  if(nr < preset_pr_bank) {
    new_preset = (preset_bank_next * preset_pr_bank) + nr;
  }

  // update bank number
  preset_bank_current = preset_bank_next;
  preset_state = PS_PRESET;
  // Debug
  char buf[20];
  int num;

  num = sprintf(buf, "Load preset(%d)\r\n", new_preset);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
}
/*
 * @brief Return current IA state
 */
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
/*
 * @brief Set IA state to ON
 */
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
  ia_t const*ia = 0;
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
/*
 * @brief Set IA state to OFF
 */
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
  ia_t const*ia = 0;
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
