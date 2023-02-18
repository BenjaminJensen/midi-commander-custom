/*
 * fifo.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */

#ifndef INC_FIFO_H_
#define INC_FIFO_H_

#include <stdint.h>

typedef struct {
  uint32_t head;
  uint32_t tail;
  uint32_t mask;
  uint32_t *buffer;
  uint32_t size;

} fifo_handler_t;

#endif /* INC_FIFO_H_ */
