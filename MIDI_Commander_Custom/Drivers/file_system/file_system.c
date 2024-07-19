/*
 * filesystem.c
 *
 *  Created on: 16 Jul 2024
 *      Author: ben
 */

#include "file_system.h"
#include "../interfaces/logging.h"
#include "flash_storage.h"
#include "hal_flash_wrapper.h"
#include "settings_types.h"
#include "settings_factory.h"

/****************************************
 * Memory
 ***************************************/

/****************************************
 * Local variables
 ***************************************/
static const uint32_t page_size_def = 2048;

static fs_memory_setup_t settings_memory = {
    .page_size = page_size_def,
    .storage_start = FLASH_START,
    .page0 = (FLASH_START + page_size_def*1),
    .page1 = (FLASH_START + page_size_def*2),
    .var_size = sizeof(settings_t),
    .blob = &flash_wrapper_write_flash_blob,
    .write = &flash_wrapper_write_flash,
    .erase = &flash_wrapper_erase_page,
    .read = &flash_wrapper_read
};

/****************************************
 * Private function declarations
 ***************************************/

static void file_system_memory_setup();

/****************************************
 * Public functions
 ***************************************/



void file_system_init() {
  log_msg("Initializing file-system...\n");
  const settings_t * s;
  s = get_settings_factory();

  file_sys_init();

  file_system_memory_setup();
}

static void file_system_memory_setup() {
  const settings_t * s;
  s = get_settings_factory();
  settings_t read_back;

  fs_initialize_page(&settings_memory);
  for(int k = 0; k < 20; k++) {
    log_msg("Memory test (%d)\n", k);
    // TEST
    fs_write_variable(&settings_memory, 1, (uint8_t*)s);

    fs_read_variable(&settings_memory, 1, (uint8_t*)&read_back);

    uint8_t* org = (uint8_t*)s;
    uint8_t* new = (uint8_t*)&read_back;

    for(int i = 0; i < settings_memory.var_size; i++) {
      if(org[i] != new[i]) {
        log_msg("FS ERROR: i(%i) o(%x), n(%x)\n", i, org[i], new[i]);
      }
    }
  }
  // TEST END
  log_msg("File-system: Memory setup complete\n");
}
