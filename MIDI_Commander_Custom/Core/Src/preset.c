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
#include "display.h"
#include "../interfaces/logging.h"
#include "menu_modifier.h"

/****************************************
 * Private types and variables
 ***************************************/

static const uint8_t preset_pr_bank = 3;
static const uint8_t preset_bank_max = 16; // 16 banks of 4 presets, total 64 presets
#pragma pack(push,1)
static preset_t preset_current = {0}; // Current state of current preamp
#pragma pack(pop)
static preset_t preset_original = {0}; // As loaded from memory
static uint8_t preset_number_current = 0;
static uint8_t preset_number_abs_current = 0;
static uint8_t preset_bank_current = 0;
static uint8_t preset_bank_next = 0;
static uint8_t preset_selected_pc = 0;
static uint8_t preset_suppress_events = 0;

enum preset_state_e {PS_PRESET, PS_IA0, PS_IA1, PS_IA2, PS_BANK, PS_PC};
static enum preset_state_e preset_state = PS_PRESET;

/****************************************
 * Private functions prototypes
 ***************************************/

static void ia_on (uint8_t nr, ia_t const*ia);
static void ia_off (uint8_t nr, ia_t const*ia);
static uint8_t ia_get_state(uint8_t nr);
static void ia_change_state(uint8_t nr, uint8_t state);

static void preset_bank_up(void);
static void preset_bank_down(void);
static void preset_load_relativ(uint8_t nr);
static void preset_save();

static void preset_update_display(void);
static void preset_bank_display(int bank);
static void preset_handle_bank(event_t e);
static void preset_page_next(void);
static void preset_handle_ia(event_t e, uint8_t offset);
static uint8_t preset_edited(void);
static void preset_handle_preset(event_t e);

static void preset_send_midi(preset_t *preset);
static void preset_copy_current_to_original();
// Edit PC functions
static void handle_exit_pc_edit();
static void preset_handle_pc(event_t e);

/****************************************
 * Public functions
 ***************************************/
/*
 * @brief Initializes the preset module and registers all event handlers
 */
void preset_init() {
  settings_init();
}


/*
 * @brief Event handler for all button events
 */
int preset_process_event(event_t e) {

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
    case PS_PC:
      preset_handle_pc(e);
      break;
    default:
      break;
  }

  disp_iax_t data;
  data.leds = 0;
  data.edited = preset_edited();

  ia_t const* ia = 0;

  pc_t const* pc = 0;
  // Show resulting state
  switch(preset_state) {
     case PS_PRESET:
       preset_update_display();
       break;
     case PS_IA0:
       for(int i = 0 ; i < 8; i++) {
         settings_get_ia(i, &ia);
         data.ias[i].id = (const char*)&(ia->id);
         if(preset_current.ia0_7 & (1 << i)) {
           data.leds |= (1 << i);
         }
       }
       data.name = "IA 1-8";
       display_iax_display(&data);
       break;
     case PS_IA1:
       for(int i = 0 ; i < 8; i++) {
         settings_get_ia(i + 8, &ia);
         data.ias[i].id = (const char*)&(ia->id);
         if(preset_current.ia8_15 & (1 << i)) {
           data.leds |= (1 << i);
         }
       }
       data.name = "IA 9-16";
       display_iax_display(&data);
       break;
     case PS_IA2:
       for(int i = 0 ; i < 8; i++) {
         settings_get_ia(i + 16, &ia);
         data.ias[i].id = (const char*)&(ia->id);
         if(preset_current.ia16_23 & (1 << i)) {
           data.leds |= (1 << i);
         }
       }
       data.name = "IA 17-24";
       display_iax_display(&data);
       break;
     case PS_BANK:
       preset_bank_display(preset_bank_next);
       break;
     case PS_PC:
       settings_get_pc(preset_selected_pc, &pc);
       display_preset_pc(
           preset_selected_pc,
           preset_current.pc[preset_selected_pc],
           pc->name);
       break;
     default:
       break;
   }

  return 0;
}

/****************************************
 * Private functions
 ***************************************/

/*
 * @brief Compares preset as loaded from memory with current state of preset
 */
static uint8_t preset_edited() {
  uint8_t edited = 0;

  if(preset_current.ia0_7 != preset_original.ia0_7) {
    edited |= 1;
  }
  if(preset_current.ia8_15 != preset_original.ia8_15) {
    edited |= 1;
  }
  if(preset_current.ia16_23 != preset_original.ia16_23) {
    edited |= 1;
  }
  return edited;
}

static void preset_copy_current_to_original() {
  preset_original.ia0_7 = preset_current.ia0_7;
  preset_original.ia8_15 = preset_current.ia8_15;
  preset_original.ia16_23 = preset_current.ia16_23;
  for(int i = 0; i < NUM_PC; i++) {
    preset_original.pc[i] = preset_current.pc[i];
  }
}
/*
 * @brief Saves the current preset state
 */
static void preset_save() {
    // Save preset
  settings_save_preset(preset_number_abs_current, &preset_current);


  preset_copy_current_to_original();

  // Reset state
  preset_suppress_events = 0;
  preset_state = PS_PRESET;
}
/*
 * @brief Update display on "Preset" page
 */
static void preset_update_display() {
  disp_preset_t p;
  p.bank = preset_bank_current;

  for(int i = 0; i < 5; i++)
    p.pc[i] = preset_current.pc[i];

  p.leds = 0;
  // Set LED states
  if(preset_current.ia0_7 & 0x01)
    p.leds |= DISP_LED_IA0;
  if(preset_current.ia0_7 & 0x02)
    p.leds |= DISP_LED_IA1;
  if(preset_current.ia0_7 & 0x04)
    p.leds |= DISP_LED_IA2;
  if(preset_current.ia0_7 & 0x08)
    p.leds |= DISP_LED_IA3;
  // Riskey but works
  p.leds |= 1 << preset_number_current;

  display_show_preset(&p);
}

/*
 * @brief update display with bank info
 */
static void preset_bank_display(int bank) {
  display_bank_display(bank);
}

/*
 *
 */
static void preset_ia_button(uint8_t ia_nr, event_t e) {
  ia_t const*ia = 0;
  settings_get_ia(ia_nr, &ia);

  if(e.event.type == EVENT_BUTTON_PRESS) {
    // Handle latch mode
    if(ia->mode == IA_MODE_LATCH) {
      if(ia_get_state(ia_nr) == 0) {
        ia_change_state(ia_nr, 1); // Current state==0 -> New State 1
        ia_on(ia_nr, ia);
      }
      else {
        ia_change_state(ia_nr, 0);// Current state==1 -> New State 0
        ia_off(ia_nr, ia);
      }
    }
    else if(ia->mode == IA_MODE_TOGGLE) {
      // IA_MODE_TOGGLE always sets state on PRESS
      ia_change_state(ia_nr, 1);
      ia_on(ia_nr, ia);
    }
  }
  else if(e.event.type == EVENT_BUTTON_RELEASE) {
    if(ia->mode == IA_MODE_TOGGLE) {
      // IA_MODE_TOGGLE always clears state on PRESS
      ia_change_state(ia_nr, 0);
      ia_off(ia_nr, ia);
    }
  }
}
/*
 * @brief Button handler for state "preset"
 */
static void preset_handle_preset(event_t e) {
  if(e.event.type == EVENT_BUTTON_HOLD) {
    log_msg("Preset: HOLD(%d)\n", e.event.data0);
  }

  switch(e.event.data0) {
    case 0:
      preset_ia_button(0, e);
      break;
    case 1:
      preset_ia_button(1, e);
      break;
    case 2:
      preset_ia_button(2, e);
      break;
    case 3:
      preset_ia_button(3, e);
      break;
    case 4:
       if(e.event.type == EVENT_BUTTON_PRESS) {
         preset_page_next();
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
      if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_bank_down();
      }
      break;
    case 9:
      if(e.event.type == EVENT_BUTTON_HOLD) {
        preset_state = PS_PC;
        preset_suppress_events = 1;
      }
      else if(e.event.type == EVENT_BUTTON_RELEASE) {
        // This handles a suppression from handle_ia: preset_save
        if(preset_suppress_events == 1) {
          preset_suppress_events = 0;
        }
        else {
          preset_bank_up();
        }
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
      if(e.event.type == EVENT_BUTTON_RELEASE) {
        preset_bank_down();
      }
      break;
    case 9:
      if(e.event.type == EVENT_BUTTON_RELEASE) {
        // This handles a suppression from handle_ia: preset_save
        if(preset_suppress_events == 1) {
          preset_suppress_events = 0;
        }
        else {
          preset_bank_up();
        }
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
  uint8_t ia_num = 255;

  switch(e.event.data0) {

    case 0:
      ia_num = 0;
      break;
    case 1:
      ia_num = 1;
      break;
    case 2:
      ia_num = 2;
      break;
    case 3:
      ia_num = 3;
      break;
    case 4:
       if(e.event.type == EVENT_BUTTON_PRESS) {
         preset_page_next();
       }
       break;

    case 5:
      ia_num = 4;
      break;
    case 6:
      ia_num = 5;
      break;
    case 7:
      ia_num = 6;
      break;
    case 8:
      ia_num = 7;
      break;
    case 9:
      if(e.event.type == EVENT_BUTTON_HOLD) {
        preset_save();
        // Ensure "BUTTON_RELEAESE" is not handled
        preset_suppress_events = 1;
        // settings_save_preset(preset_number_abs_current, &preset_current);
      }
      break;
    default:
      break;
  }
  // If IA button, call handler
  if( ia_num != 255) {
    preset_ia_button(ia_num + (8 * offset), e);
    /*
    if(e.event.type == EVENT_BUTTON_PRESS) {
      preset_ia(ia_num + (8 * offset), 1);
    }
    else if(e.event.type == EVENT_BUTTON_RELEASE) {
      preset_ia(ia_num + (8 * offset), 0);
    }
    */
  }
}

/*
 * @handle Program Change edit mode in preset
 */
static void preset_handle_pc(event_t e) {
  if(preset_selected_pc > 4) {
    log_msg("WARNING(preset_handle_pc): out of range[%d]", preset_selected_pc);
    preset_selected_pc = 0;
  }
  switch(e.event.data0) {
    case 0: // -1
      if(e.event.type == EVENT_BUTTON_PRESS) {
        mm8_sub1(&(preset_current.pc[preset_selected_pc]), 0);
      }
      break;
    case 1: // +1
      if(e.event.type == EVENT_BUTTON_PRESS) {
        mm8_add1(&(preset_current.pc[preset_selected_pc]), 127);
      }
      break;
    case 2: // -10
      if(e.event.type == EVENT_BUTTON_PRESS) {
        mm8_sub10(&(preset_current.pc[preset_selected_pc]), 0);
      }
      break;
    case 3: // +10
      if(e.event.type == EVENT_BUTTON_PRESS) {
        mm8_add10(&(preset_current.pc[preset_selected_pc]), 127);
      }
      break;
    case 4:
      // EXIT
      if(e.event.type == EVENT_BUTTON_RELEASE) {
        handle_exit_pc_edit();
      }
      break;

    case 5: // +10
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_selected_pc = 0;
      }
      break;

    case 6: // +10
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_selected_pc = 1;
      }
      break;

    case 7: // +10
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_selected_pc = 2;
      }
      break;

    case 8: // +10
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_selected_pc = 3;
      }
      break;

    case 9: // +10
      if(e.event.type == EVENT_BUTTON_PRESS) {
        preset_selected_pc = 4;
      }
      break;
    default:
     log_msg("ERROR(preset_handle_pc): unknown data0 element(%d) \n", e.event.data0);
     break;
  }
  //log_msg("preset_handle_pc: pc%d = %d\n", preset_selected_pc, preset_current.pc[preset_selected_pc]);
}

static void handle_exit_pc_edit() {

  preset_save();
  // Update display
  preset_update_display();
  log_msg("Exit PC edit mode\n");

}
/*
 * @brief Next preset page event handler
 */
static void preset_page_next() {
  /*

  // Tempory disable extra IA pages
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
      */
  switch(preset_state) {
    case PS_PRESET:
      preset_state = PS_IA0;
      break;
    case PS_IA0:
      preset_state = PS_IA1;
      break;
    default:
      preset_state = PS_PRESET;
      break;
  }
  log_msg("Next Page:%d\n", preset_state);
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
  log_msg("Bank up c:%d n:%d\r\n", preset_bank_current, preset_bank_next);
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
  log_msg("Bank down c:%d n:%d\r\n", preset_bank_current, preset_bank_next);
}

/*
 *
 */

static void preset_clear(preset_t *preset) {
  preset->crc = 0;
  for(int i = 0; i < NUM_PC; i++)
    preset->pc[i] = 0;

  preset->ia0_7 = 0;
  preset->ia8_15 = 0;
  preset->ia16_23 = 0;
}
/*
 * @brief Loads preset relative to current "bank"
 */
static void preset_load_relativ(uint8_t nr) {
  uint8_t new_preset = 0;
  int error = 0;

  if(nr < preset_pr_bank) {
    new_preset = (preset_bank_next * preset_pr_bank) + nr;
    preset_number_current = nr;
    preset_number_abs_current = new_preset;
  }
  else {
    log_msg("ERROR(preset_load_relativ): nr() >= preset_pr_bank()\n", nr, preset_pr_bank);
    log_msg("Loading preset '%d' instead\n", new_preset);
  }

  error = settings_load_preset(new_preset, &preset_current);

  if(error == 0) {
    preset_copy_current_to_original();
    // update bank number
    preset_bank_current = preset_bank_next;
    preset_state = PS_PRESET;
    log_msg("Loading preset '%d'\n", new_preset);
    preset_send_midi(&preset_current);

  }
  else if(error == -2) { // No saved preset
    // Reset preset
    preset_clear(&preset_current);
    // Set an empty preset for saving
    // TODO: Find a flow for saving presets to empty locations and/or creating presets in empty locations
    preset_bank_current = preset_bank_next;
    preset_state = PS_PRESET;
    log_msg("preset_load_relativ: preset(%d) not available\n", preset_number_abs_current);
  }
  else {
    log_msg("ERROR(preset_load_relativ): Load preset(%d) failed(%d)!\r\n", new_preset, error);
  }
}
/*
 *
 */
static void ia_change_state(uint8_t nr, uint8_t state) {
  uint8_t *reg = 0;
  uint8_t mask = 0;

  // Select reg and calc mask
  if( nr < 8 ) {
    reg = &(preset_current.ia0_7);
    mask = 1 << nr;
  }
  else if( nr < 16 ) {
    reg = &(preset_current.ia8_15);
    mask = 1 << (nr - 8);
  }
  else if( nr < 24 ) {
    reg = &(preset_current.ia16_23);
    mask = 1 << (nr - 16);
  }

  // Update state
  if(state == 0) {
    *reg &= ~mask;
  }
  else {
    *reg |= mask;
  }
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
static void ia_on (uint8_t nr, ia_t const*ia) {
  if(ia != 0) {
    if(ia->type == IA_TYPE_CC) {
        midi_send_cc(ia->midi_chan, ia->midi_data0, ia->midi_data1);
      }
  }

  // Debug
  log_msg("IA on(%d)\r\n", nr);
}
/*
 * @brief Set IA state to OFF
 */
static void ia_off (uint8_t nr, ia_t const*ia) {
  if(ia != 0) {
    if(ia->type == IA_TYPE_CC && ia->midi_data2 <= 0x7F) {
      midi_send_cc(ia->midi_chan, ia->midi_data0, ia->midi_data2);
    }
  }

  // Debug
  log_msg("IA off(%d)\r\n", nr);
}

/*
 * @brief sends preset MIDI data when a new preset is loaded
 *
 * NOTE: It will FIRST send Program Change followed by Continues Control
 *       This is done to ensurer CC's can modify newly loaded presets on external units
 */
static void preset_send_midi(preset_t *preset) {
   // Send Program Chaneg - PC
  pc_t const* pc_s = 0;
  for(int i = 0; i < NUM_PC; i++) {
    settings_get_pc(i, &pc_s);
    if(pc_s->mode == PC_MODE_ON) {
      midi_send_pc(pc_s->chan, preset->pc[i]);
    }
  }

  // Send Controller Change - CC
  ia_t const* ia_s;
  for(int i = 0; i < NUM_IA; i++) {
    settings_get_ia(i, &ia_s);
    if(ia_s->type == IA_TYPE_CC) {
      // Evaluate resulting state
      if(ia_get_state(i) == 0) {
        ia_off(i, ia_s);
      }
      else {
        ia_on(i, ia_s);
      }
    }
    else if(ia_s->type == IA_TYPE_PC) {
      // TODO: Figure out how should work and implement
      //midi_send_pc(pc_s->chan, );
    }
  }
}
