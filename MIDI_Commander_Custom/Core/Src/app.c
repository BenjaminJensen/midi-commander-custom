/*
 * app.c
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */

#include "stm32f1xx_it.h"
#include "scheduler.h"
#include <stdio.h>
#include "SEGGER_RTT.h"

static task_tcb_t task1_tcb;
static task_tcb_t task2_tcb;

static void task1(void);
static void task2(void);

void app_init() {
  task_create(&task1_tcb, 1000, &task1);
  task_start(&task1_tcb);
  task_create(&task2_tcb, 500, &task2);
  task_start(&task2_tcb);
}



void app_run() {


  task_scheduler_run();
  // scan buttons 10ms

  // process events
}

static void task1(void) {
  static int cnt = 0;

  char buf[32];
  int num = sprintf(buf, "Task 1 (%d)\r\n", cnt);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
  cnt++;
}

static void task2(void) {
  static int cnt = 0;

  char buf[32];
  int num = sprintf(buf, "Task 2 (%d)\r\n", cnt);
  buf[num] = 0;
  SEGGER_RTT_WriteString(0, buf);
  cnt++;
}
