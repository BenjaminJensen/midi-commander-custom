/*
 * hal_flash_wrapper.h
 *
 *  Created on: 3 Sep 2023
 *      Author: ben
 */

#ifndef INC_HAL_FLASH_WRAPPER_H_
#define INC_HAL_FLASH_WRAPPER_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f103xe.h" // Defines: "FLASH_BANK1_END"
#include "stm32f1xx_hal_flash_ex.h" // Defines: "FLASH_PAGE_SIZE"
/*
 * Flash defines
 */
#define FLASH_NUM_PAGES (10)
#define FLASH_START     (FLASH_BANK1_END - FLASH_PAGE_SIZE*FLASH_NUM_PAGES + 1)

/*
 * Flash wrapper exports
 */
int flash_wrapper_write_flash(uintptr_t addr, uint16_t data);
int flash_wrapper_write_flash_blob(uintptr_t addr, uint8_t* data_p, uint32_t size);
int flash_wrapper_read(uintptr_t addr, uint8_t* data_p, uint32_t size);
int flash_wrapper_erase_page(uintptr_t page_start);

#endif /* INC_HAL_FLASH_WRAPPER_H_ */
