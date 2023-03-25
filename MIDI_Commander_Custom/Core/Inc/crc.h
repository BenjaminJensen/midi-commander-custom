/*
 * crc.h
 *
 *  Created on: 19 Mar 2023
 *      Author: ben
 */

#ifndef INC_CRC_H_
#define INC_CRC_H_

#include <stdint.h>

uint16_t crc16_calc(uint8_t *input, int length);
#endif /* INC_CRC_H_ */
