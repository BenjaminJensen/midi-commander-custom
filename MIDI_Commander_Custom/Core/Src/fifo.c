/*
 * fifo.c
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */


#include "fifo.h"

int fifo_init(fifo_handler_t *handle, uint32_t mask, uint32_t *buffer) {
  handle->tail = 0;
  handle->head = 0;
  handle->mask = mask;
  handle->size = mask - 1;
  handle->buffer = buffer;

  return 0;
}

int fifo_put(fifo_handler_t *handle, uint32_t item) {
  uint32_t tmphead;
  uint32_t error = 0;

  // calculate buffer index
  tmphead = ( handle->head + 1) & handle->mask;

  if ( tmphead == handle->tail ) {
    // error buffer full
    error = -1;


  }
  else {
    // store new index
    handle->head = tmphead;
    // store received data in buffer
    handle->buffer[tmphead] = item;
  }
  return error;
}

int fifo_get(fifo_handler_t *handle, uint32_t *intem_out) {
  uint32_t tmptail;
  int error = 0;

  if ( handle->head == handle->tail ) {
    error = -2;
  }
  else {
    // calculate /store buffer index
    tmptail = (handle->tail + 1) & handle->mask;
    handle->tail = tmptail;

    // get data from receive buffer
    *intem_out = handle->buffer[tmptail];
  }

  return error;
}
