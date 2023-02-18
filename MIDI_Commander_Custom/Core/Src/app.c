/*
 * app.c
 *
 *  Created on: Feb 18, 2023
 *      Author: ben
 */

#include "stm32f1xx_it.h"

void app_init() {

}

uint32_t app_get_cnt_diff(uint32_t new, uint32_t old) {
  uint32_t diff = 0;
  if(old < new ) {
    diff = new - old;
  }
  else if(old > new) {
    diff = ( 0xFFFFFFFF - old) + new;
  }

  return diff;
}

void app_run() {
  static uint32_t cnt_old = 0;
  static uint32_t cnt = 0;
  static uint32_t but_cnt = 0;
  uint32_t diff;
  cnt = get_systic_counter();
  diff = app_get_cnt_diff(cnt, cnt_old);

  if(diff > 0) {
    but_cnt += diff;

    if(but_cnt >= 10) {
      //uint16_t but = buttons_scan();
    }


    cnt_old = cnt;
  }

  // scan buttons 10ms

  // process events
}
