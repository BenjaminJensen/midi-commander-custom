/*
 * event.c
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#include <stdio.h>
#include "event.h"
#include "fifo.h"
#include "preset.h"
#include "display.h"
#include "SEGGER_RTT.h"

static fifo_handler_t event_queue_handle;
static uint32_t event_queue_data[32];

enum system_mode_e {MODE_PRESET, MODE_IA0, MODE_IA1};

static enum system_mode_e system_mode = MODE_PRESET;


static void event_handle_button(event_t e);

void event_init() {
  fifo_init(&event_queue_handle, 0x1F,event_queue_data );
  preset_init();
}

int event_put(event_t e) {
  int error = 0;

  if(fifo_put(&event_queue_handle, e.data) != FIFO_OK) {
    error = -1;
  }
  return error;
}

void event_process() {
  char buf[32];
  int num;

  event_t e;
  while(fifo_get(&event_queue_handle, &e.data) == FIFO_OK) {
    switch(e.event.type) {
      case EVENT_BUTTON_PRESS:
      case EVENT_BUTTON_RELEASE:
      case EVENT_BUTTON_LONG:
        event_handle_button(e);
        break;
      default:
        num = sprintf(buf, "Unknown event %d, %d, %d, %d\r\n", e.event.type, e.event.data0, e.event.data2, e.event.data2);
        buf[num] = 0;
        SEGGER_RTT_WriteString(0, buf);
    }
  }
  display_update();
}





static void event_handle_button_preset(event_t e) {
  switch(e.event.data0) {
    case 0:
      if(system_mode == MODE_PRESET) {
        system_mode = MODE_IA0;
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
  preset_update_display();
}

static void event_handle_button(event_t e) {
  if(system_mode == MODE_PRESET) {
    event_handle_button_preset(e);
  }
}

