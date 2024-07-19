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
  /*
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  */
  int ret;

  va_list args;
  va_start(args, format);
  ret = SEGGER_RTT_vprintf(0, format, &args);
  va_end(args);

  return ret;
}
