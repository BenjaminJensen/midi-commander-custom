/*
 * scheduler.c
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */

/**********************************
 * Includes
**********************************/
#include "scheduler.h"
#include "stm32f1xx_hal.h"

extern void ssd1306_tick(void);
/**********************************
 * Defines
**********************************/
#define NUM_TASKS (8)

/**********************************
 * Private variables
**********************************/

static task_tcb_t* tasks[NUM_TASKS] = {0};

static volatile uint32_t systic_counter = 0;

static int tasks_created = 0;

/**********************************
 * Public functions
**********************************/
/**
* @brief  Creates a new task in the scheduler
*         The task starts in STOPED mode
* @param task_tcb_t Task Control Block pointer
* @param  freq task Frequency in systic clocks
* @retval int
*/
int task_create(task_tcb_t *handle, uint32_t freq,  task_fp func) {
  uint32_t error = 0;

  if(tasks_created < NUM_TASKS)
  {
    tasks[tasks_created] = handle;
    tasks[tasks_created]->freq = freq;
    tasks[tasks_created]->state = TASK_STOPPED;
    tasks[tasks_created]->task_function = func;
    tasks_created++;
  }
  else {
    // No free task slots
  }

  return error;
}
/**
* @brief  Starts the task
* @param task_tcb_t Task Control Block pointer
* @retval int
*/
int task_start(task_tcb_t *handle) {
  handle->next = systic_counter;
  handle->state = TASK_RUN;

  return 0;
}
/**
* @brief  Called from main to execute tasks
*/
void task_scheduler_run(void) {
  task_tcb_t *tcb;

  for(int t = 0; t < tasks_created; t++) {
    tcb = tasks[t];
    if(tcb->state == TASK_RUN ) {
      // update TCB
      tcb->next = tcb->next + tcb->freq;
      tcb->state = TASK_RUNNING;
      // Run actual task
      (*tcb->task_function)();
      // Set task state back to waiting
      tcb->state = TASK_WAITING;
    }
  }
}

/**
* @brief  Called from systic to update task states
*/
void task_scheduler_update() {
  task_tcb_t *tcb;

  for(int t = 0; t < tasks_created; t++) {
    tcb = tasks[t];
    if((tcb->state == TASK_WAITING) && ( systic_counter >= tcb->next)) {
      tcb->state = TASK_RUN;
    }
  }
}
/**
  * @brief This function handles System tick timer.
  * Called by the ST CMSIS layer
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  //ssd1306_tick();
  systic_counter++;
  task_scheduler_update();
  /* USER CODE END SysTick_IRQn 1 */
}
