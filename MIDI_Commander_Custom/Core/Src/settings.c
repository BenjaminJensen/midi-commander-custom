/*
 * settings.c
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */

#include "settings.h"
#include <stdio.h>
#include "crc.h"
#include "../interfaces/logging.h"
#include "settings_factory.h"
#include "file_system.h"


/****************************************
 * Memory
 ***************************************/

 /****************************************
  * Local variables
  ***************************************/
static settings_t settings_current = {0} ;

/****************************************
 * Private function declarations
 ***************************************/

static int settings_preset_nr_to_addr(uint8_t nr);
static void settings_load_system();

/****************************************
 * Public functions
 ***************************************/
/*
 * @brief Initialize the settings module
 */
void settings_init() {

  file_system_init();
  settings_load_system();
  //settings_current = get_settings_factory();
  // validate loaded settings
  // if not valid
  //    Load factory settings
}

/*
 * @brief Get the settings for a specific IA
 */
int settings_get_ia(uint8_t nr, ia_t const** ia) {
  int error = 0;
  if(nr < NUM_IA) {
    *ia = &(settings_current.ias[nr]);
  }
  else {
    ia = 0;
    error = -1;
  }
  return error;
}

/*
 * @brief Get the settings for a specific PC
 */
int settings_get_pc(uint8_t nr, pc_t const** pc) {
  int error = 0;
  if(nr < NUM_PC) {
    *pc = &(settings_current.pcs[nr]);
  }
  else {
    pc = 0;
    error = -1;
  }
  return error;
}

/*
 * @brief Load a preset from the designated number
 */
int settings_load_preset(int nr, preset_t* preset) {
  int error = 0;
  uint16_t crc = 0;

  error = file_system_load_preset(nr, preset);
  if(error == 0) {
    log_msg("Loading preset(%d), CRC: %x\n", nr, preset->crc);
    crc = crc16_calc((uint8_t*)preset + 2, sizeof(preset_t) - 2);

    if(crc != preset->crc) {
      error = -1;
      log_msg("Load preset(CRC error) load: %d != %d\n", preset->crc, crc);
    }
  }
  else if(error != -2) { // Disregard missing preset
    log_msg("ERROR(settings_load_preset): unable to load preset(%d)\n", error);
  }

  return error;
}

/*
 * @brief Save a preset to the designated number
 */
int settings_save_preset(int nr, preset_t* preset) {
  int error = 0;

  preset->crc = crc16_calc((uint8_t*)preset + 2, sizeof(preset_t) - 2);

  error = file_system_store(nr, preset);

  if(error == 0) {
    log_msg("Write preset: nr: %d, crc: %x\n", nr, preset->crc);
  }
  else {
    log_msg("ERROR(settings_save_preset): unable to save preset(%d)\n", error);
  }

  return error;
}
/****************************************
 * Private function declarations
 ***************************************/

/*
 * @brief Load system setting from memory or write and load default
 */
static void settings_load_system() {
  int error;
  error = file_system_load_settings(&settings_current);
  if(error != 0) {
    // Unable to load system settings
    // File does not exist, write default settings
    if(error == -2) {
      file_system_store_settings((settings_t*)get_settings_factory());
      error = file_system_load_settings(&settings_current);
    }
    if(error != 0) {
      log_msg("settings_load_system: FATAL ERROR\n", error);
      while(1);
    }
  }
  if(error == 0) {
    log_msg("System settings loaded(%X)\n", error);
  }
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
