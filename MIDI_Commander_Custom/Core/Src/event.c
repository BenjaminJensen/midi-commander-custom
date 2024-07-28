/*
 * event.c
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#include <stdio.h>
#include "event.h"
#include "fifo.h"
#include "display.h"
#include "SEGGER_RTT.h"

/****************************************
 * Local variables
 ***************************************/
typedef struct {
  enum event_type_e type;
  event_handler_f fp;
} event_handler_t;

static fifo_handler_t event_queue_handle;
static uint32_t event_queue_data[32];
#define EVENT_NUM_HANDLERS (8)
static const int event_max_handlers = EVENT_NUM_HANDLERS;
static int event_num_handlers = -1;
static event_handler_t event_handlers[EVENT_NUM_HANDLERS] = {0};

/****************************************
 * Public functions
 ***************************************/
/*
 *
 */
void event_init() {
  fifo_init(&event_queue_handle, 0x1F,event_queue_data );
}
/*
 *
 */
int event_handler_register(enum event_type_e e, event_handler_f fp) {
  int error = -1;

  // -1 == none, move to first slot
  if(event_num_handlers == -1)
    event_num_handlers = 0;

  if(event_num_handlers < event_max_handlers) {
    event_handlers[event_num_handlers].type = e;
    event_handlers[event_num_handlers].fp = fp;
    event_num_handlers++;
    error = 0;
  }
  return error;
}
/*
 *
 */
int event_put(event_t e) {
  int error = 0;

  if(fifo_put(&event_queue_handle, e.data) != FIFO_OK) {
    error = -1;
  }
  return error;
}

void event_process() {

  event_t e;
  while(fifo_get(&event_queue_handle, &e.data) == FIFO_OK) {
    // Any handlers registered?
    if(event_num_handlers > 0) {
      // Loop through event handlers
      for(int i = 0; i < event_num_handlers; i++) {
        // Call event handlers for the given event type
        if(event_handlers[i].type == e.event.type) {
          event_handlers[i].fp(e);
        }
      }
    }
  }
  // Send display buffer to display after proccessing events
  display_update();
}
