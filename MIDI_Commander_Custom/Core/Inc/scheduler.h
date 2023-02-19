/*
 * scheduler.h
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

enum task_state_e {TASK_STOPPED, TASK_STARTED, TASK_WAITING, TASK_RUN, TASK_RUNNING};
typedef void (*task_fp)(void);

typedef struct {
  uint32_t last;
  uint32_t next;
  uint32_t freq;
  enum task_state_e state;
  task_fp task_function;
} task_tcb_t;

int task_create(task_tcb_t *handle, uint32_t freq, task_fp func);
void task_scheduler_run(void);
int task_start(task_tcb_t *handle);

#endif /* INC_SCHEDULER_H_ */
