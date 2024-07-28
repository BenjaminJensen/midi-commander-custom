/*
 * system_state.c
 *
 *  Created on: 28 Jul 2024
 *      Author: ben
 */

/****************************************
 * Includes
 ***************************************/
#include "system_state.h"
#include "event.h"
#include "../interfaces/logging.h"
#include "preset.h"
/****************************************
 * Private types and variables
 ***************************************/
static struct {
  enum system_state_e state;
} system;

/****************************************
 * Private functions prototypes
 ***************************************/

static int system_process_event(event_t e);

/****************************************
 * Public functions
 ***************************************/

void ss_init() {
  system.state = SS_PRESET;
  preset_init();

  event_handler_register(EVENT_BUTTON_PRESS, &system_process_event);
  event_handler_register(EVENT_BUTTON_RELEASE, &system_process_event);
  event_handler_register(EVENT_BUTTON_HOLD, &system_process_event);
}

void ss_set_state(enum system_state_e state) {
  system.state = state;
}

/****************************************
 * Private functions
 ***************************************/

static int system_process_event(event_t e) {
  switch(system.state) {
    case SS_PRESET:
      preset_process_event(e);
      break;
    case SS_SETINGS:
      break;
    default:
      break;
  }
  return 0;
}
