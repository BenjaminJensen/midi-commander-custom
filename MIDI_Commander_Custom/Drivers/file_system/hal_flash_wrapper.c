/*
 * hal_flash_wrapper.c
 *
 *  Created on: 26 Jul 2023
 *      Author: ben
 *
 *  Flash programming manual    https://www.st.com/resource/en/programming_manual/pm0075-stm32f10xxx-flash-memory-microcontrollers-stmicroelectronics.pdf
 */


#include "hal_flash_wrapper.h"
#include "stm32f1xx_hal_flash.h"
#include "../interfaces/logging.h"
#include "SEGGER_RTT.h"
static int file_sys_test();

int file_sys_init() {
  file_sys_test();
  return 0;
}

int flash_wrapper_write_flash(uintptr_t addr, uint16_t data) {
  int error = 0;
  HAL_FLASH_Unlock();

  error = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, (uint64_t)data);

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
  for(uint32_t i = 0; i < size; i++) {
    data_p[i] = *(__IO uint8_t *)(addr + i);
  }
  return 0;
}
/*
 *
 */
int flash_wrapper_erase_page(uintptr_t page_start) {
  FLASH_EraseInitTypeDef erase_conf;
  uint32_t flash_error = 0;

  HAL_FLASH_Unlock();

  int error = 0;
  erase_conf.Banks = 0;
  erase_conf.NbPages = 1;
  erase_conf.PageAddress = page_start;
  erase_conf.TypeErase = FLASH_TYPEERASE_PAGES;

  // Erase Page0
  int hal_status = HAL_FLASHEx_Erase(&erase_conf, &flash_error);

  HAL_FLASH_Lock();

  if(hal_status != HAL_OK) {
    error = -1;
    log_msg("FST: Flash erase ERROR, HAL_STATUS: %x\n", hal_status);
  }
  if(flash_error != 0xFFFFFFFF) {
    error = -2;
    log_msg("FST: Flash erase ERROR: %x\n", flash_error);
  }

  return error;
}

/****************************************
 * Private functions
 ***************************************/

static int file_sys_test() {
  int error;
  log_msg("File system low level test\n");
  uint32_t offset = 0;

  log_msg("FS: Erase at: %x\n", FLASH_START + offset);
  log_msg("end: %x\n", FLASH_BANK1_END);
  log_msg("size: %x\n", 2048*32);
  SEGGER_RTT_printf(0, "\nSegger %x (%x - %x)\n", FLASH_START, FLASH_BANK1_END, 2048*32);
  error = flash_wrapper_erase_page(FLASH_START + offset);
  if(error != 0) {
    log_msg("FST: ERROR Unable to erase page: %x\n", error);
    return error;
  }

  error = flash_wrapper_write_flash(FLASH_START + offset, 0xFACE);
  if(error != 0) {
    log_msg("FST: ERROR Unable to write: %x\n", error);
    return error;
  }

  uint16_t data;
  error = flash_wrapper_read(FLASH_START + offset, (uint8_t*)&data, 2);
  if(data != 0xFACE) {
    log_msg("FST: ERROR did not read what was written: %x != %x\n", data, 0xFACE);
  }
  else {
    log_msg("FST: Readback OK\n");
  }
  return 0;
}
