/*
 * fifo.c
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */


#include "fifo.h"
#include <assert.h>

int fifo_init(fifo_handler_t *handle, uint32_t mask, uint32_t *buffer) {
   // Assert that (mask + 1) is a power of two
  assert(((mask + 1) & mask) == 0 && "FIFO mask+1 must be a power of two!");

  handle->tail = 0;
  handle->head = 0;
  handle->mask = mask;
  handle->size = mask - 1;
  handle->buffer = buffer;

  return 0;
}

int fifo_put(fifo_handler_t *handle, uint32_t item) {
  uint32_t tmphead;
  uint32_t error = FIFO_OK;

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

int fifo_get(fifo_handler_t *handle, uint32_t *item_out) {
  uint32_t tmptail;
  int error = FIFO_OK;

  if ( handle->head == handle->tail ) {
    error = -2;
  }
  else {
    // calculate /store buffer index
    tmptail = (handle->tail + 1) & handle->mask;
    handle->tail = tmptail;

    // get data from receive buffer
    *item_out = handle->buffer[tmptail];
  }

  return error;
}
