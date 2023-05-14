/*
 * flash_storage.h
 *
 *  Created on: 1 Apr 2023
 *      Author: ben
 */

#ifndef INC_FLASH_STORAGE_H_
#define INC_FLASH_STORAGE_H_

#include <stdint.h>


int flash_storage_init(void);
int flash_storage_write(uint16_t addr, uint8_t *data, uint16_t size);
int flash_storage_read(uint16_t addr, uint8_t *data, uint16_t size);
int fs_write_preset(int nr, uint16_t *data);

#endif /* INC_FLASH_STORAGE_H_ */
