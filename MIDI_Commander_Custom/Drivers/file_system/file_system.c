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
#include "settings_factory.h"
#include <assert.h>

/****************************************
 * Constants
 ***************************************/
static const uint16_t settings_v_addr = 1;
static const uint8_t presets_pr_page = 100;
static const uint32_t page_size_def = 2048;

/****************************************
 * Local variables
 ***************************************/
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

static fs_memory_setup_t preset_memory0 = {
    .page_size = page_size_def,
    .storage_start = FLASH_START,
    .page0 = (FLASH_START + page_size_def*3),
    .page1 = (FLASH_START + page_size_def*4),
    .var_size = sizeof(preset_t),
    .blob = &flash_wrapper_write_flash_blob,
    .write = &flash_wrapper_write_flash,
    .erase = &flash_wrapper_erase_page,
    .read = &flash_wrapper_read
};

static fs_memory_setup_t preset_memory1 = {
    .page_size = page_size_def,
    .storage_start = FLASH_START,
    .page0 = (FLASH_START + page_size_def*5),
    .page1 = (FLASH_START + page_size_def*6),
    .var_size = sizeof(preset_t),
    .blob = &flash_wrapper_write_flash_blob,
    .write = &flash_wrapper_write_flash,
    .erase = &flash_wrapper_erase_page,
    .read = &flash_wrapper_read
};

/****************************************
 * Private function declarations
 ***************************************/

static void file_system_memory_setup();
static fs_memory_setup_t* file_system_get_preset_setup(uint16_t nr);

/****************************************
 * Public functions
 ***************************************/

void file_system_init() {
  log_msg("Initializing file-system...\n");
  if((sizeof(preset_t) * presets_pr_page)  > (page_size_def / 2)) {
    log_msg("ERROR: Too many presets pr page!\n");
  }

  file_sys_init();

  file_system_memory_setup();
}
/*
 *
 */
int file_system_load_settings(settings_t* settings) {
  int error;

  error = fs_read_variable(&settings_memory, settings_v_addr, (uint8_t*)settings);

  return error;
}
/*
 *
 */
int file_system_store_settings(settings_t* settings) {
  int error = 0;
  error = fs_write_variable(&settings_memory, settings_v_addr, (uint8_t*)settings);

  return error;
}
/*
 *
 */
int file_system_load_preset(uint16_t nr, preset_t* preset) {
  int error = 0;
  fs_memory_setup_t* fs_p;
  fs_p = file_system_get_preset_setup(nr);


  // No errors, load preset
  if(fs_p != 0) {
    error = fs_read_variable(fs_p, nr, (uint8_t*)preset);
  }

  return error;
}
/*
 *
 */
int file_system_store(uint16_t nr, preset_t* preset) {
  int error = 0;
  fs_memory_setup_t* fs_p;
  fs_p = file_system_get_preset_setup(nr);

  // No errors, load preset
  if(fs_p != 0) {
    error = fs_write_variable(fs_p, nr, (uint8_t*)preset);
  }
  return error;
}

/****************************************
 * Private functions
 ***************************************/
static fs_memory_setup_t* file_system_get_preset_setup(uint16_t nr) {
  fs_memory_setup_t* fs_p = 0;
  if(nr <= (presets_pr_page * 1) ) {
    fs_p = &preset_memory0;
  }
  else if(nr <= (presets_pr_page * 2) ) {
    fs_p = &preset_memory1;
  }
  else {
    log_msg("ERROR: preset number out of range: %d", nr);
  }
  return fs_p;
}

static void file_system_test() {
  const settings_t * s;
  s = get_settings_factory();
  settings_t read_back;

  for(int k = 0; k < 20; k++) {
    log_msg("Memory test (%d)\n", k);
    // TEST
    fs_write_variable(&settings_memory, settings_v_addr, (uint8_t*)s);

    fs_read_variable(&settings_memory, settings_v_addr, (uint8_t*)&read_back);

    uint8_t* org = (uint8_t*)s;
    uint8_t* new = (uint8_t*)&read_back;

    for(int i = 0; i < settings_memory.var_size; i++) {
      if(org[i] != new[i]) {
        log_msg("FS ERROR: i(%i) o(%x), n(%x)\n", i, org[i], new[i]);
      }
    }
  }
  // TEST END
}

static void file_system_memory_setup() {
  // System settings memory
  fs_initialize_page(&settings_memory);

  // Preset memory
  // TODO: setup

  log_msg("File-system: Memory setup complete\n");
}
