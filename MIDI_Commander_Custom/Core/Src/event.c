/*
 * event.c
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#include <stdio.h>
#include "event.h"
#include "fifo.h"

#include "SEGGER_RTT.h"

static fifo_handler_t event_queue_handle;
static uint32_t event_queue_data[32];

void event_init() {
  fifo_init(&event_queue_handle, 0x1F,event_queue_data );
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


  event_t e;
  while(fifo_get(&event_queue_handle, &e.data) == FIFO_OK) {
    int num = sprintf(buf, "event %d, %d, %d, %d\r\n", e.event.type, e.event.data0, e.event.data2, e.event.data2);
    buf[num] = 0;
    SEGGER_RTT_WriteString(0, buf);
  }
}
