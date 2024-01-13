/*
 * logging.c
 *
 *  Created on: 26 Jul 2023
 *      Author: ben
 */

#include "../interfaces/logging.h"

#include <stdarg.h>
#include "SEGGER_RTT.h"

int log_msg(const char *format, ...) {
  int ret;
  ret = SEGGER_RTT_printf(0, format);

  return ret;
}
