/*
 * flash_storage.c
 *
 *  Created on: 1 Apr 2023
 *      Author: ben
 *
 * Implementation based on application note: AN2594
 * https://www.st.com/resource/en/application_note/an2594-eeprom-emulation-in-stm32f10x-microcontrollers-stmicroelectronics.pdf
 *
 * Some inspiration taken from: https://github.com/snashpo/treckker/blob/master/eeprom.c
 * -------------------------------------------------------------
 * 2 bytes for page status
 * Settings 508 bytes + 2 byte virtual address = 510 bytes = 1+1 pages
 *
 * Preset:
 * - 2 bytes crc
 * - 8 bytes data
 * - 16 bytes name
 * - 2 bytes virtual address
 * = 28 bytes
 * 28 * 36 = 1008
 *
 * 4 pages * 36 presets = 144 presets = 4x2x2048 = 16kb
 * --------------------------------------------------------------
 * Preset: size 28 + 2 byte virtual address
 *
 * --------------------------------------------------------------
 * 7 "double pages" will follow the strategy from the application note
 *
 * Each page will be initialized
 */
#include "flash_storage.h"
#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

/****************************************
 *
 ***************************************/

/****************************************
 * Local constants
 ***************************************/
/*
 * Page 0 - 1 settings
 * Page 2 - 8 presets
 */

static const uint32_t page_size = 2048;
static const uint32_t pages = (4 + 1)*2; // ( 4 (preset) + 1 (settings) ) * 2
static const uint32_t storage_start = FLASH_BANK1_END - page_size*pages + 1;
static const uint32_t fs_preset_start = storage_start + page_size*2;
// Vasriable size includes virtual address of 16 bits!
static const uint16_t fs_preset_size = 28;

enum page_status_e {ERASED = 0xFFFF, RECEIVE_DATA = 0xEEEE, VALID_PAGE = 0x0000};
static const uint32_t no_valid_page_e = 0xFFFFFFFF;
static const uint32_t page_start_offset = 2;
#define FS_MAX_VARIABLES 36

/****************************************
 * Private function declarations
 ***************************************/
static int fs_init_erased(uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status);
static int fs_init_receive(uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status);
static int fs_init_valid(uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status);

static int fs_mark_page(uint32_t addr, uint16_t mark);
static int fs_erase_page(uint32_t page_addr);
static int fs_format(uint32_t page_0, uint32_t page_1);

static int fs_write_and_verify_variable(uint16_t virtual_address, uint16_t *data, uint16_t size, uint32_t page_0, uint32_t page_1);
static int fs_write_halfword(uint32_t addr, uint16_t data);

/****************************************
 * Public functions
 ***************************************/
int flash_storage_init(void) {
  int error = 0;
  uint16_t page_status_0;
  uint16_t page_status_1;
  uint32_t page_0_addr;
  uint32_t page_1_addr;

  // Unlock FLASH control register
  HAL_FLASH_Unlock();

  // Run through all pages and validate
  for(int page_index = 0; page_index < (pages / 2 ); page_index++) {
    page_0_addr = storage_start + page_index*2*page_size;
    page_1_addr = storage_start + page_index*2*page_size + page_size;

    SEGGER_RTT_printf(0, "FS init: p0(%d) addr %x\n", page_index, page_0_addr);
    SEGGER_RTT_printf(0, "FS init: p1(%d) addr %x\n", page_index, page_1_addr);
    // Get Page0 status
    page_status_0 = (*(__IO uint16_t*)(page_0_addr));
    // Get Page1 status
    page_status_1 = (*(__IO uint16_t*)(page_1_addr));


    SEGGER_RTT_printf(0, "FS init: p0 status %x\n", page_status_0);
    SEGGER_RTT_printf(0, "FS init: p1 status %x\n", page_status_1);

    // TODO: Debug
    //fs_format(page_0_addr, page_1_addr);

    switch(page_status_0) {
      case ERASED:
        error = fs_init_erased(page_0_addr, page_1_addr, page_status_0, page_status_1);
        if(error != 0) {
          SEGGER_RTT_printf(0, "case: ERASED failed to initialize\n");
        }
        else {
          SEGGER_RTT_printf(0, "case: ERASED page(%d) initialized\n", page_index);
        }
        break;
      case RECEIVE_DATA:
        error = fs_init_receive(page_0_addr, page_1_addr,  page_status_0,  page_status_1);
        if(error != 0) {
          SEGGER_RTT_printf(0, "case: RECEIVE_DATA failed to initialize\n");
        }
        else {
          SEGGER_RTT_printf(0, "case: RECEIVE_DATA page(%d) initialized\n", page_index);
        }
        break;
      case VALID_PAGE:
        error = fs_init_valid(page_0_addr, page_1_addr,  page_status_0,  page_status_1);
        if(error != 0) {
          SEGGER_RTT_printf(0, "case: VALID_PAGE failed to initialize\n");
        }
        else {
          SEGGER_RTT_printf(0, "case: VALID_PAGE page(%d) initialized\n", page_index);
        }
        break;
      default:
        SEGGER_RTT_printf(0, "FS init: unknown page status: %x\n", page_status_0);
        break;
    }
    SEGGER_RTT_printf(0, "\n");
  } // for loop

  // Lock FLASH control register
  HAL_FLASH_Lock();

  page_status_0 = (*(__IO uint16_t*)(page_0_addr));
  // Get Page1 status
  page_status_1 = (*(__IO uint16_t*)(page_1_addr));

  SEGGER_RTT_printf(0, "FS init: p0 status %x\n", page_status_0);
  SEGGER_RTT_printf(0, "FS init: p1 status %x\n", page_status_1);
  uint8_t data[28] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};
  for(int j = 0; j < 3; j++) {
    for(int i = 0; i < 36; i++) {
      if(fs_write_preset(i, (uint16_t*)data) != 0) {
        return -1;
      }
    }
  }
  return error;
}
/*
 *
 */
int flash_storage_write(uint16_t addr, uint8_t *data, uint16_t size) {
  int error = 0;

  return error;
}

int flash_storage_read(uint16_t addr, uint8_t *data, uint16_t size) {
  int error = 0;

  return error;
}
/*
 * 36 presets pr 2048kb page = 1008 bytes pr page
 */
int fs_write_preset(int nr, uint16_t *data) {
  int error = 0;
  uint32_t page_0 = 0xFFFFFFFF;
  uint32_t page_1 = 0xFFFFFFFF;

  if(nr < 36) {
    // page 0 + 1
    page_0 = fs_preset_start;
    page_1 = fs_preset_start + page_size * 1;
  }
  else if(nr < (36*2)) {
    // page 2 + 3
    page_0 = fs_preset_start + page_size * 2;
    page_1 = fs_preset_start + page_size * 3;
  }
  else if(nr < (36*3)) {
    // page 4 + 5
    page_0 = fs_preset_start + page_size * 4;
    page_1 = fs_preset_start + page_size * 5;
  }
  else if(nr < (36*4)) {
    // page 6 + 7
    page_0 = fs_preset_start + page_size * 6;
    page_1 = fs_preset_start + page_size * 7;
  }
  else {
    SEGGER_RTT_printf(0, "fs_write_preset: invalid preset number (%d", nr);
    error = -1;
  }

  if(error == 0) {
    error = fs_write_and_verify_variable(nr, data, fs_preset_size, page_0, page_1);
  }

  return error;
}

/****************************************
 * Private functions
 ***************************************/

static uint32_t fs_get_read_page(uint32_t page_0, uint32_t page_1) {
  uint16_t page_0_status = 6;
  uint16_t page_1_status = 6;
  uint32_t page = no_valid_page_e;

  // Get Page0 actual status */
  page_0_status = (*(__IO uint16_t*)page_0);

  // Get Page1 actual status */
  page_1_status = (*(__IO uint16_t*)page_1);

  if (page_0_status == VALID_PAGE) {
    page = page_0;
  }
  else if (page_1_status == VALID_PAGE) {
    page = page_1;
  }

  return page;
}

static uint32_t fs_get_write_page(uint32_t page_0, uint32_t page_1) {
  uint16_t page_0_status = 6;
  uint16_t page_1_status = 6;
  uint32_t page = no_valid_page_e;

  // Get Page0 actual status */
  page_0_status = (*(__IO uint16_t*)page_0);

  // Get Page1 actual status */
  page_1_status = (*(__IO uint16_t*)page_1);

  if (page_1_status == VALID_PAGE) {
    // Page0 receiving data
    if (page_0_status == RECEIVE_DATA) {
      page = page_0;
    }
    else {
      page = page_1;
    }
  }
  else if (page_0_status == VALID_PAGE) {
    // Page1 receiving data
    if (page_1_status == RECEIVE_DATA) {
      page = page_1;
    }
    else {
      page = page_0;
    }

  }

  return page;
}
/*
 * @brief Moves and consolidates data from one page to another to free up space
 */
static int fs_move_to_page(uint32_t from_page, uint32_t to_page, uint16_t size) {
  int error = 0;
  uint16_t variables[FS_MAX_VARIABLES];
  int index = 0;

  // Initialize found variables to none (=0xFFFF)
  for(int i = 0; i < FS_MAX_VARIABLES; i++){
    variables[i] = 0xFFFF;
  }
  SEGGER_RTT_printf(0, "fs_move_to_page:\n");
  // Find last possible variable address
  uint32_t last_variable_addr = from_page;
  // Find end address on last possible variable (size inc. virtual address)
  // Note: taking advantage of reminder in division disappears
  last_variable_addr += (page_size - page_start_offset)/(size) * size;
  // Add page offset
  last_variable_addr += page_start_offset;
  uint32_t next_var_addr = last_variable_addr;

  while(next_var_addr >= (from_page + page_start_offset)) {
    uint16_t v_addr;
    uint32_t v_addr_addr = next_var_addr - 2;
    v_addr = *(__IO uint16_t*)(v_addr_addr);
    int found = 0;
    for(int i = 0; i < (index + 1); i++) {
      if(variables[i] == v_addr) {
        found = 1;
        SEGGER_RTT_printf(0, "fs_move_to_page: found == 1, var: %x, a: %x,, i: %d\n", v_addr, next_var_addr, i);

      }
    }
    if(found == 0) {
      SEGGER_RTT_printf(0, "fs_move_to_page: found == 0\n");
      variables[index] = v_addr;
      index++;
    }

    // Next a variable
    next_var_addr = next_var_addr - size;
  }
  return error;
}
/*
 * @brief Write a variable to a specific memory address
 * note: the memory address needs to be erased and valid, else the function will fail
 */
static int fs_write_variable(uint32_t address, uint16_t *data, uint16_t size, uint16_t virtual_address) {
  HAL_StatusTypeDef status = HAL_ERROR;

   //status = fs_write_halfword(address + size, virtual_address);
   HAL_FLASH_Unlock();
   uint32_t v_addr_addr = address + size - 2;
   status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, v_addr_addr, virtual_address);
   if(status != 0) {
     SEGGER_RTT_printf(0, "fs_write_variable: Failed to program virtual address in flash\n");
     status = -1;
   }
   else {
     SEGGER_RTT_printf(0,"fs_write_variable: w: %d at a: %x i:%d\n", size, address, virtual_address);
     // Virtual address written, now write data, data == size -1 (1 for virtual address)
     for(int i = 0; i < (size / 2 - 1); i++) {
       uint32_t next_addr = address + (i *2);
       uint64_t next_data = data[i];
       //status = fs_write_halfword(next_addr, next_data);

       status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, next_addr, next_data);

       if(status != HAL_OK) {
         SEGGER_RTT_printf(0, "fs_write_variable: Failed to program flash #%x (%d)\n", next_addr, status);
         status = -1;
         break;
       }
       else {
         //SEGGER_RTT_printf(0, "r: %d, r: %d\n",*(__IO uint16_t*)next_addr, data[i]);
       }
     } // for(int i
   } // else

   HAL_FLASH_Lock();

   return status;
}
/*
 * @brief Write a variable to the memory system.
 * The function will:
 * 1. Find a valid page to write on
 * 2. Find valid address
 * 3. If no valid address, change page
 * 4. Write the actual data to the memory system
 */
static int fs_write_and_verify_variable(uint16_t virtual_address, uint16_t *data, uint16_t size, uint32_t page_0, uint32_t page_1) {
  uint32_t page;
  int error = 0;

  page = fs_get_write_page(page_0, page_1);

  if(page != no_valid_page_e) {
    uint32_t end_address = page + page_size - 2;
    uint32_t address = page + 2;

    // ----------------------------------------------------
    // Find valid address
    // Loop through page from beginning
    while (address < end_address && error == 0) {
      // if virtual address is free
      if ((*(__IO uint16_t*)(address + size - 2)) == 0xFFFF) {
        break;
      } // ((*(__IO uint16_t*)(address + size)) == 0xFFFF)
      else {
        address += size;
      }
    }// while(

    // TODO: get new write address on new page
    if( address > (end_address - size - 2)) {
      // If page is full move to new page and consolidate
      if(page == page_0) {
        // From page 0 to page 1
        fs_move_to_page(page_0, page_1, size);
        SEGGER_RTT_printf(0, "fs_write_and_verify_variable: move from 0 to 1\n");
      }
      else {
        // From page 1 to page 0
        fs_move_to_page(page_1, page_0, size);
        SEGGER_RTT_printf(0, "fs_write_and_verify_variable: move from 1 to 0\n");
      }
    }

    // TODO: Needs fixing
    // ----------------------------------------------------
    // If valid address
    if( address <= (end_address - size)) {
      fs_write_variable(address, data, size, virtual_address);
    } // if( address <= (end_address - size - 2))
    else {
      error = -2;
      SEGGER_RTT_printf(0, "fs_write_variable: end of page!\n");
    }
    // ----------------------------------------------------
  }
  else {
    SEGGER_RTT_printf(0, "fs_write_variable: no valid page\n");
    error = -1;
  }

  return error;
}

/*
 * @brief Slimmed down version of the ST HAL FLASH_Program_HalfWord
 * NOTE: Is tailored to devices with two flash banks 0 and 1
 */

static int fs_write_halfword(uint32_t addr, uint16_t data) {
  int error = 0;
  HAL_StatusTypeDef hal_status = HAL_OK;

  // Process Locked
  hal_status = HAL_FLASH_Lock();

  if(hal_status == HAL_OK) {

    // Proceed to program the new data
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    // Write data in the address
    *(__IO uint16_t*)addr = data;

    // Process Unlocked
    HAL_FLASH_Unlock();
  }
  else {
    error = -1;
    SEGGER_RTT_printf(0, "fs_write_halfword: unable to lock flash!\n");
  }

  return error;
}
/*
 *
 */
static int fs_init_valid(uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status) {
  int error = 0;
  // Invalid state -> format eeprom
  if (page_1_status == VALID_PAGE) {

     // Erase both Page0 and Page1 and set Page0 as valid page
     fs_format(page_0, page_1);
     //TODO: check error
  }
  else { // if (PageStatus1 == VALID_PAGE)
    // Page0 valid, Page1 erased
    if (page_1_status == ERASED) {

    /* Erase Page1 */
      fs_erase_page(page_1);
    // TODO: Error handling
    }
    else
    { // if (PageStatus1 == ERASED)
      // Page0 valid, Page1 receive

      // TODO: Transfer data from Page0 to Page1

      // Mark Page1 as valid
      fs_mark_page(page_1, VALID_PAGE);
      // TODO: error handling

      // Erase Page0
      fs_erase_page(page_0);
    } // if (PageStatus1 == VALID_PAGE)
  }
  return error;
}
/*
 *
 */
static int fs_init_receive(uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status) {
  int error = 0;

  // Page0 receive, Page1 valid
  if (page_1_status == VALID_PAGE) {
    // TODO:
    // Transfer data from Page1 to Page0

    // Mark Page0 as valid
    error = fs_mark_page(page_0, VALID_PAGE);
    //TODO: Check error

    // Erase Page1
    error = fs_erase_page(page_1);
    //TODO: Check error

  }
  else { // if (PageStatus1 == VALID_PAGE)

    // Page0 receive, Page1 erased
    if (page_1_status == ERASED) {
      // Erase Page1
      error = fs_erase_page(page_1);
      //TODO: Check error

      // Mark Page0 as valid
      error = fs_mark_page(page_0, VALID_PAGE);
      //TODO: Check error
    }
    else { // if (PageStatus1 == ERASED)
      // Invalid state -> format eeprom

      fs_format(page_0, page_1);

    } /* if (PageStatus1 == ERASED) */
  } /* if (PageStatus1 == VALID_PAGE) */

  return error;
}

/*
 *
 */
static int fs_erase_page(uint32_t page_addr) {
  FLASH_EraseInitTypeDef erase_conf;
  uint32_t flash_error = 0;
  int error = 0;
  erase_conf.Banks = 0;
  erase_conf.NbPages = 1;
  erase_conf.PageAddress = page_addr;
  erase_conf.TypeErase = FLASH_TYPEERASE_PAGES;

  // Erase Page0
  HAL_FLASHEx_Erase(&erase_conf, &flash_error);
  if(flash_error != 0xFFFFFFFF) {
    error = -1;
  }

  return error;
}

/*
 *
 */
static int fs_format(uint32_t page_0, uint32_t page_1) {
  int error = 0;

  // Erase Page0
  error = fs_erase_page(page_0);
  if(error == 0) {
    SEGGER_RTT_printf(0, "fs_format programming VALID_PAGE:\n");
    HAL_StatusTypeDef status = HAL_ERROR;

    status = fs_mark_page(page_0, VALID_PAGE);

    if(status != HAL_OK) {
      SEGGER_RTT_printf(0, "fs_format: Failed to program flash\n");
    }

    error = fs_erase_page(page_1);
  }
  return error;
}

/*
 *
 */
static int fs_init_erased(uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status) {
  int error = 0;
  //uint16_t  flash_status;

  // Page0 erased, Page1 valid
  if (page_1_status == VALID_PAGE) {
    error = fs_erase_page(page_0);
    if(error != 0) {
      // Failed to erase page 0
      SEGGER_RTT_printf(0, "fs_init_erase: failed to erase page 0");
    }
  }
  else { // if (PageStatus1 == VALID_PAGE)
    // Page0 erased, Page1 receive
    if (page_1_status == RECEIVE_DATA) {
      // Erase Page0
      error = fs_erase_page(page_0);
      if(error == 0) {

        SEGGER_RTT_printf(0, "fs_init_erased programming: VALID_PAGE:\n");
        // Mark Page1 as valid
        HAL_StatusTypeDef status = HAL_ERROR;

        status = fs_mark_page(page_1, VALID_PAGE);

        if(status != HAL_OK) {
           SEGGER_RTT_printf(0, "fs_init_erased: Failed to program flash\n");
         }
      }
      else {
        // Failed to erase page 0
      }
    }
    else { // if (PageStatus1 == RECEIVE_DATA)
      /*
      * First EEPROM access (Page0&1 are erased) or
      * invalid state -> format EEPROM
      */

      SEGGER_RTT_printf(0, "fs_init_erased: formatting:\n");
      // Erase both Page0 and Page1 and set Page0 as valid page
      error = fs_format(page_0, page_1);
      if(error != 0) {
        // Failed to format pages
      }

    } // if (PageStatus1 == RECEIVE_DATA)
  } // if (PageStatus1 == VALID_PAGE)
  return error;
}

/*
 *
 */

static int fs_mark_page(uint32_t addr, uint16_t mark) {
  int error = 0;
  // Mark Page
  HAL_StatusTypeDef status = HAL_ERROR;
  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, mark);
  if(status != HAL_OK) {
     SEGGER_RTT_printf(0, "fs_mark_page: Failed to program flash\n");
     error = -1;
   }
  return error;
}
