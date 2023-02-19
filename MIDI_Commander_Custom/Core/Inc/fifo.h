/*
 * fifo.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */

#ifndef INC_FIFO_H_
#define INC_FIFO_H_

#include <stdint.h>

enum fifo_e {FIFO_OK = 0};
typedef struct {
  uint32_t head;
  uint32_t tail;
  uint32_t mask;
  uint32_t *buffer;
  uint32_t size;

} fifo_handler_t;

int fifo_init(fifo_handler_t *handle, uint32_t mask, uint32_t *buffer);
int fifo_put(fifo_handler_t *handle, uint32_t item);
int fifo_get(fifo_handler_t *handle, uint32_t *intem_out);

#endif /* INC_FIFO_H_ */
