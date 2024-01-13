/*
 * logging.h
 *
 *  Created on: 26 Jul 2023
 *      Author: ben
 */

#ifndef INTERFACES_LOGGING_H_
#define INTERFACES_LOGGING_H_

#if __cplusplus
extern "C" {
#endif

int log_msg(const char *format, ...);

#if __cplusplus
}
#endif

#endif /* INTERFACES_LOGGING_H_ */
