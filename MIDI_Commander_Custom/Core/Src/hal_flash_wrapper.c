/*
 * hal_flash_wrapper.c
 *
 *  Created on: 26 Jul 2023
 *      Author: ben
 */


#include "hal_flash_wrapper.h"
#include "stm32f1xx_hal_flash.h"
#include "../interfaces/logging.h"

int flash_wrapper_write_flash(uintptr_t addr, uint16_t data) {
  int error = 0;
  HAL_StatusTypeDef hal_status = HAL_OK;

  // Unlock FLASH control register
  hal_status = HAL_FLASH_Unlock();

  if(hal_status == HAL_OK) {

    // Proceed to program the new data
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    // Write data in the address
    *(volatile uint16_t*)addr = data;
  }
  else {
    error = -1;
    log_msg("flash_wrapper_write_flash: unable to lock flash!\n");
  }
  // Lock FLASH control register
  HAL_FLASH_Lock();

  return error;
}

int flash_wrapper_write_flash_blob(uintptr_t addr, uint8_t* data_p, uint32_t size) {
  int error = 0;
  HAL_StatusTypeDef hal_status = HAL_OK;

  // Unlock FLASH control register
  hal_status = HAL_FLASH_Unlock();
  if(hal_status == HAL_OK) {
    if(size & 0x1) {
      log_msg("flash_wrapper_write_flash_blob: WARNING size is uneven\n");
    }
    // Proceed to program the new data
    SET_BIT(FLASH->CR, FLASH_CR_PG);
    uint16_t * data16 = (uint16_t*)data_p;
    for(int i = 0; i < (size / 2); i++) {
      // Write data in the address
      *(volatile uint16_t*)(addr + i*2) = data16[i];
    }
  }
  else {
    error = -1;
    log_msg("flash_wrapper_write_flash_blob: unable to lock flash!\n");
  }
  // Lock FLASH control register
  HAL_FLASH_Lock();

  return error;
}

int flash_wrapper_read(uintptr_t addr, uint8_t* data_p, uint32_t size) {

  return 0;
}
/*
 *
 */
int flash_wrapper_erase_page(uintptr_t page_start) {
  FLASH_EraseInitTypeDef erase_conf;
  uint32_t flash_error = 0;
  int error = 0;
  erase_conf.Banks = 0;
  erase_conf.NbPages = 1;
  erase_conf.PageAddress = page_start;
  erase_conf.TypeErase = FLASH_TYPEERASE_PAGES;

  // Erase Page0
  HAL_FLASHEx_Erase(&erase_conf, &flash_error);
  if(flash_error != 0xFFFFFFFF) {
    error = -1;
  }

  return error;
}
