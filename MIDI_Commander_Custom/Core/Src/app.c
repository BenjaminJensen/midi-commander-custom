/*
 * app.c
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */

/****************************************
 * Includes
 ***************************************/
#include "event.h"
#include "system_state.h"

/****************************************
 * Public functions
 ***************************************/

/*
 * @brief Initialize the application
 */
void app_init() {

  event_init();
  ss_init();
}

/*
 * @brief Run the application
 */
void app_run() {
  // process events
  event_process();
}
