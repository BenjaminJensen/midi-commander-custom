/*
 * menu_modifier.c
 *
 *  Created on: 25 Jul 2024
 *      Author: ben
 */
#include "menu_modifier.h"

void mm8_add1(uint8_t* val, uint8_t max) {
  if(*val < (max - 1) && max <= 255) {
    *val += 1;
  }
  else {
    *val = max;
  }
}
void mm8_add10(uint8_t* val, uint8_t max) {
  if(*val < (max - 10) && max <= 245) {
    *val += 10;
  }
  else {
    *val = max;
  }
}

void mm8_sub1(uint8_t* val, uint8_t min) {
  if(*val > (min + 1)) {
    *val -= 1;
  }
  else {
    *val = min;
  }
}
void mm8_sub10(uint8_t* val, uint8_t min) {
  if(*val > (min + 10)) {
    *val -= 10;
  }
  else {
    *val = min;
  }
}
