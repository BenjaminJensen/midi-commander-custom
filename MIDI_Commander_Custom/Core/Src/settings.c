/*
 * settings.c
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */

#include "settings.h"
#include <stdio.h>
#include "crc.h"
#include "flash_storage.h"
#include "../interfaces/logging.h"
#include "settings_factory.h"
#include "hal_flash_wrapper.h"


/****************************************
 * Memory
 ***************************************/
#define MEMORY_NUM_SEGMENTS (5)
static fs_memory_setup_t memory_segments[MEMORY_NUM_SEGMENTS] = {
    { // Settings storage
        .page_size = FLASH_PAGE_SIZE,
        .storage_start = FLASH_START + FLASH_PAGE_SIZE*2*0, // two pages pr segment, offset by number of segments
        .page0 = FLASH_START + FLASH_PAGE_SIZE*2*0,
        .page1 = FLASH_START + FLASH_PAGE_SIZE + FLASH_PAGE_SIZE*2*0,
        .var_size = sizeof(settings_t),
        .blob = &flash_wrapper_write_flash_blob,
        .write = &flash_wrapper_write_flash,
        .erase = flash_wrapper_erase_page,
        .read = 0
    },
    { // Preset storage 0
        .page_size = FLASH_PAGE_SIZE,
        .storage_start = FLASH_START + FLASH_PAGE_SIZE*2*1, // two pages pr segment, offset by number of segments
        .page0 = FLASH_START + FLASH_PAGE_SIZE*2*1,
        .page1 = FLASH_START + FLASH_PAGE_SIZE + FLASH_PAGE_SIZE*2*1,
        .var_size = sizeof(preset_t),
        .blob = &flash_wrapper_write_flash_blob,
        .write = &flash_wrapper_write_flash,
        .erase = flash_wrapper_erase_page,
        .read = 0
    },
    { // Preset storage 1
        .page_size = FLASH_PAGE_SIZE,
        .storage_start = FLASH_START + FLASH_PAGE_SIZE*2*2, // two pages pr segment, offset by number of segments
        .page0 = FLASH_START + FLASH_PAGE_SIZE*2*2,
        .page1 = FLASH_START + FLASH_PAGE_SIZE + FLASH_PAGE_SIZE*2*2,
        .var_size = sizeof(preset_t),
        .blob = &flash_wrapper_write_flash_blob,
        .write = &flash_wrapper_write_flash,
        .erase = flash_wrapper_erase_page,
        .read = 0
    },
    { // Preset storage 2
        .page_size = FLASH_PAGE_SIZE,
        .storage_start = FLASH_START + FLASH_PAGE_SIZE*2*3, // two pages pr segment, offset by number of segments
        .page0 = FLASH_START + FLASH_PAGE_SIZE*2*3,
        .page1 = FLASH_START + FLASH_PAGE_SIZE + FLASH_PAGE_SIZE*2*3,
        .var_size = sizeof(preset_t),
        .blob = &flash_wrapper_write_flash_blob,
        .write = &flash_wrapper_write_flash,
        .erase = flash_wrapper_erase_page,
        .read = 0
    },
    { // Preset storage 3
        .page_size = FLASH_PAGE_SIZE,
        .storage_start = FLASH_START + FLASH_PAGE_SIZE*2*4, // two pages pr segment, offset by number of segments
        .page0 = FLASH_START + FLASH_PAGE_SIZE*2*4,
        .page1 = FLASH_START + FLASH_PAGE_SIZE + FLASH_PAGE_SIZE*2*4,
        .var_size = sizeof(preset_t),
        .blob = &flash_wrapper_write_flash_blob,
        .write = &flash_wrapper_write_flash,
        .erase = flash_wrapper_erase_page,
        .read = 0
    },
};

 /****************************************
  * Local variables
  ***************************************/
static settings_t const *settings_current = 0 ;

/****************************************
 * Private function declarations
 ***************************************/

static int settings_preset_nr_to_addr(uint8_t nr);
static void hal_dump_mem(uint8_t *buf, uint8_t size);

/****************************************
 * Public functions
 ***************************************/
/*
 * @brief Initialize the settings module
 */
void settings_init() {

  // validate loaded settings
  // if not valid
  //    Load factory settings
  //(flash_start, flash_write_short single, flash_write_blob blob, flash_erase_page erase)
  //flash_storage_init(FLASH_START, &flash_wrapper_write_flash, &flash_wrapper_write_flash_blob, &flash_wrapper_erase_page);

  if(fs_read_variable(&memory_segments[0], 0, 0) != 0) {
    settings_current = get_settings_factory();
  }
}
/*
 * @brief Get the settings for a specific IA
 */
int settings_get_ia(uint8_t nr, ia_t const**ia) {
  int error = 0;
  if(nr < NUM_IA) {
    *ia = &(settings_current->ias[nr]);
  }
  else {
    ia = 0;
    error = -1;
  }
  return error;
}
/*
 * @brief Load a preset from the designated number
 */
int settings_load_preset(int nr, preset_t* preset) {
  int error = 0;
  int addr;
  uint16_t crc = 0;
  addr = settings_preset_nr_to_addr(nr);

  //flash_storage_read(addr, (uint8_t*)preset, 10);
  //hal_eeprom_bulk_read(addr, (uint8_t*)preset, 10);
  crc = crc16_calc((uint8_t*)preset + 2, 8);
  log_msg("settings_load_preset: nr: %d, addr: %d\n", nr, addr);
  if(crc != preset->crc) {
    error = -1;
    hal_dump_mem((uint8_t*)preset, 10);
    log_msg("Load preset(crc error) load: %d != %d\n", preset->crc, crc);
  }
  return error;
}
/*
 * @brief Save a preset to the designated number
 */
int settings_save_preset(int nr, preset_t* preset) {
  int error = 0;
  uint16_t addr;

  // Align adress to eeprom pages
  addr = settings_preset_nr_to_addr(nr);

  // Calc CRC
  preset->crc = crc16_calc((uint8_t*)preset + 2, 8);

  log_msg("Write preset\naddr: %d, nr: %d, crc: %x\n", addr, nr, preset->crc);

  flash_storage_write(addr, (uint8_t*)preset, 10);

  return error;
}
/****************************************
 * Private function declarations
 ***************************************/
static void hal_dump_mem(uint8_t *buf, uint8_t size) {
  for(int i = 0; i < size; i++) {
    log_msg("%x, ", buf[i]);
  }
  log_msg("\n");
}
/*
 *
 */
static int settings_preset_nr_to_addr(uint8_t nr) {
  int addr;
  if(nr < 14) {
    // Page 2
    addr = 116 + 256; // Addr 115 on page 2
    // Add preset offset
    addr += nr * 10;
  }
  else if(nr < 39) {
    // Start of page 3
    addr = 256*2;
    // Add preset offset
    addr += (nr - 14) * 10;
  }
  else {
    // Start of page 4
    addr = 256*3;
    // Add preset offset
    addr += (nr - 39) * 10;
  }
  return addr;
}
