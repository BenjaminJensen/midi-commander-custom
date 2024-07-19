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
static settings_t const *settings_current = 0 ;

/****************************************
 * Private function declarations
 ***************************************/

static int settings_preset_nr_to_addr(uint8_t nr);

/****************************************
 * Public functions
 ***************************************/
/*
 * @brief Initialize the settings module
 */
void settings_init() {

  file_system_init();
  settings_current = get_settings_factory();
  // validate loaded settings
  // if not valid
  //    Load factory settings
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
    //hal_dump_mem((uint8_t*)preset, 10);
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

  log_msg("Write preset\naddr: %d, nr: %d, crc: %x\n", addr, nr, preset->crc);


  return error;
}
/****************************************
 * Private function declarations
 ***************************************/

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
