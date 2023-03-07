/*
 * event.h
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#ifndef INC_EVENT_H_
#define INC_EVENT_H_

#include <stdint.h>

enum event_type_e {EVENT_BUTTON_PRESS, EVENT_BUTTON_RELEASE, EVENT_BUTTON_LONG};

typedef struct {
    uint8_t type;
    uint8_t data0;
    uint8_t data1;
    uint8_t data2;
  } event_struct_t;

typedef union {
  uint32_t data;
  event_struct_t event;
} event_t;
typedef int (*event_handler_f)(event_t e);

void event_init(void);
void event_process(void);
int event_put(event_t e);
int event_handler_register(enum event_type_e e, event_handler_f fp);

#endif /* INC_EVENT_H_ */
