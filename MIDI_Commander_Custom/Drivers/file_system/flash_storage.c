/*
 * flash_storage.c
 *
 *  Created on: 1 Apr 2023
 *      Author: ben
 *
 * Implementation based on application note: AN2594
 * https://www.st.com/resource/en/application_note/an2594-eeprom-emulation-in-stm32f10x-microcontrollers-stmicroelectronics.pdf
 * https://github.com/STMicroelectronics/STM32CubeF1/blob/master/Projects/STM32F103RB-Nucleo/Examples/FLASH/FLASH_EraseProgram/Src/main.c#L83
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
#include "../interfaces/logging.h"

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


static fs_memory_setup_t fs_mem_map ={
    .page_size = 2048,
    //.pages = (4 + 1)*2, // ( 4 (preset) + 1 (settings) ) * 2
    .storage_start = 0,//FLASH_BANK1_END - page_size*pages + 1;
};

//static fs_memory_setup_t* fs_ = &fs_mem_map;

// Variable size includes virtual address of 16 bits!
static const uint16_t fs_preset_size = 28;

enum page_status_e {ERASED = 0xFFFF, RECEIVE_DATA = 0xEEEE, VALID_PAGE = 0x0000};
static const uintptr_t no_valid_page_e = 0x0;
// offset of 2 bytes needed for page status: page_status_e
static const uint32_t page_start_offset = 2;
#define FS_MAX_VARIABLES 36

// Virtual address size in bytes
static const unsigned int v_addr_size = 2;

/****************************************
 * Private function declarations
 ***************************************/
// All moved to the section for "Internal Public functions"


/****************************************
 * Internal Public functions (public for testing purpose)
 ***************************************/
/*
* @brief Sets up the internal data structure for the flash storage
*/
fs_memory_setup_t *fs_setup_memory(uintptr_t flash_start, flash_write_short single, flash_write_blob blob, flash_erase_page erase) {

  // Setup HW pointers
  fs_mem_map.write = single;
  fs_mem_map.blob = blob;
  fs_mem_map.erase = erase;

  fs_mem_map.storage_start = flash_start;

  /*
  fs_ = &fs_mem_map;

  if(single == 0) {
    log_msg("sigle == null\n");
    fs_ = 0;
  }

  if(blob == 0) {
    log_msg("blob == null\n");
    fs_ = 0;
  }

  if(erase == 0) {
    log_msg("erase == null\n");
    fs_ = 0;
  }

  return fs_;
  */
 return 0;
}

/*
* @brief Initializes a set of pages ready for storage.
* Use as asetup function for a memory area
*/
int fs_initialize_page(fs_memory_setup_t *fs_p) {
  int error;
  
  uint16_t page_status_0;
  uint16_t page_status_1;
  uint16_t *tmp;
  
  // Get Page0 status
  tmp = (void *) fs_p->page0;
  page_status_0 = *tmp;
  // Get Page1 status
  tmp = (void *) fs_p->page1;
  page_status_1 = *tmp;


  switch(page_status_0) {
    case ERASED:
      error = fs_init_erased(fs_p, fs_p->page0, fs_p->page1, page_status_0, page_status_1);
      if(error != 0) {
        log_msg("case: ERASED failed to initialize\n");
      }
      else {
        log_msg("case: ERASED page(%d) initialized\n", fs_p->page0);
      }
      break;
    case RECEIVE_DATA:
      error = fs_init_receive(fs_p, fs_p->page0, fs_p->page1,  page_status_0,  page_status_1);
      if(error != 0) {
        log_msg("case: RECEIVE_DATA failed to initialize\n");
      }
      else {
        log_msg("case: RECEIVE_DATA page(%d) initialized\n", fs_p->page0);
      }
      break;
    case VALID_PAGE:
      error = fs_init_valid(fs_p, fs_p->page0, fs_p->page1,  page_status_0,  page_status_1);
      if(error != 0) {
        log_msg("case: VALID_PAGE failed to initialize\n");
      }
      else {
        log_msg("case: VALID_PAGE page(%d) initialized\n", fs_p->page0);
      }
      break;
    default:
      log_msg("FS init: unknown page status: %x\n", page_status_0);
      error = -10;
      break;
  }

  return error;
}

/****************************************
 * Public functions
 ***************************************/

int fs_read_variable(fs_memory_setup_t *fs_p, uint16_t v_addr, uint8_t *data) {
  int error = 0;

  // Find active page
  uintptr_t page = fs_get_write_page(fs_p);
  if(page == no_valid_page_e) {
    log_msg("fs_read_variable: no valid page!\n");
    return -1;
  }

  // Find variable
  uintptr_t var_addr = fs_locate_variable(fs_p, page, v_addr, fs_p->var_size);

  if(var_addr != 0) {
    // copy data
    error = fs_p->read(var_addr, data, fs_p->var_size);
  }
  else {
    log_msg("fs_read_variable: variable not found!\n");
    error = -2;
  }
  return error;
}

int fs_write_variable(fs_memory_setup_t *fs_p, uint16_t v_addr, uint8_t *data) {
  int error;

  error = fs_write_and_verify_variable(fs_p, v_addr, data, fs_p->var_size);

  return error;
}

/****************************************
 * Private functions
 ***************************************/

uintptr_t fs_get_read_page(fs_memory_setup_t *fs_p) {
  uint16_t page_0_status = 6;
  uint16_t page_1_status = 6;
  uintptr_t page = no_valid_page_e;

  // Get Page0 actual status */
  page_0_status = (*(volatile uint16_t*)fs_p->page0);

  // Get Page1 actual status */
  page_1_status = (*(volatile uint16_t*)fs_p->page1);

  if (page_0_status == VALID_PAGE) {
    page = fs_p->page0;
  }
  else if (page_1_status == VALID_PAGE) {
    page = fs_p->page1;
  }

  return page;
}

uintptr_t fs_get_write_page(fs_memory_setup_t *fs_p) {
  uintptr_t page = no_valid_page_e;
  enum fs_storage_mode_e mode = fs_get_storage_mode(fs_p);
  
  switch (mode)
  {
    case PAGE0_VALID:
      page = fs_p->page0;
      break;
    case PAGE1_VALID:
      page = fs_p->page1;
      break;
    
    default:
      break;
  }
  return page;
}

/*
* @locate the address of newest version of a virtual address
*/
uintptr_t fs_locate_variable(fs_memory_setup_t *fs_p, uintptr_t page, uint16_t v_addr, uint32_t size) {
  uintptr_t addr = 0;
  // Calculate offset at end of page
  unsigned int end_offset = (fs_p->page_size - page_start_offset) % (size + v_addr_size);
  // First available address
  uintptr_t next_addr = page + fs_p->page_size - end_offset - size - v_addr_size;

  //log_msg("end offset: %d \n", end_offset);
  // Validate virtual address is valid
  if(v_addr != 0xFFFF) {
    while(next_addr > page) {
      // do stuff
      uint16_t *cur_v_addr = (uint16_t*)(next_addr + size);
      // log_msg("next_addr: %x, relative: %x\n", next_addr, next_addr - page);
      if(*cur_v_addr == v_addr) {
        addr = next_addr;
        break;
      }
      // update next address 
      next_addr = next_addr - size - v_addr_size;
    }
  }
  else {
    log_msg("fs_locate_variable: virtual address can not be '0'!\n");
  }

  return addr;
}
/*
* @brief
*/
int fs_add_unique_to_array(uint16_t * array, uint16_t value, uint16_t array_size) {
  int added = 0;
  int i = 0;

  for(; i < array_size; i++) {
    if(array[i] == 0xFFFF) {
      break;
    }
    else if(array[i] == value) {
      added = -1;
      break;
    }
  }
  if(i < array_size) {
    if(added == 0) {
      array[i] = value;
    }
  }
  else {
    log_msg("fs_add_unique_to_array: array out of bound!\n");
    added = -1;
  }

  //log_msg("V: %x, index: %d\n", value, i);
  return added;
}

/*
* @brief Returns the current storage mode base page0 and page1 status
*/
// ERASED = 0xFFFF, RECEIVE_DATA = 0xEEEE, VALID_PAGE = 0x0000
enum fs_storage_mode_e fs_get_storage_mode(fs_memory_setup_t *fs_p) {
  enum fs_storage_mode_e mode = INVALID;

  uint16_t page0_status = *((uint16_t*)(fs_p->page0));
  uint16_t page1_status = *((uint16_t*)(fs_p->page1));

  switch(page0_status) {
    case ERASED:
      // Page0 == ERASED
      switch(page1_status) {
        case RECEIVE_DATA:
          mode = PAGE1_MARK_VALID;
          break;
        case VALID_PAGE:
          mode = PAGE1_VALID;
          break;
      };
      break;
    case RECEIVE_DATA:
      // page0 RECEIVE_DATA
      switch(page1_status) {
        case ERASED:
          mode = PAGE0_MARK_VALID;
          break;
        case VALID_PAGE:
          mode = PAGE0_RECEIVING;
          break;
      };
      break;
    case VALID_PAGE:
      // page0 RECEIVE_DATA
      switch(page1_status) {
        case ERASED:
          mode = PAGE0_VALID;
          break;
        case RECEIVE_DATA:
          mode = PAGE1_RECEIVING;
          break;
      };
      break;
  };
  return mode;
}

/*
 * @brief Moves and consolidates data from one page to another to free up space
* |---------------------------------------------------------
* |
*
*
 */
int fs_move_to_page(fs_memory_setup_t *fs_p, uintptr_t from_page, uintptr_t to_page, uint32_t size) {
  int error = 0;
  uint8_t max_num_vars = 68;
  uint16_t variables[68];

  // Initialize found variables to none (=0xFFFF)
  for(int i = 0; i < max_num_vars; i++){
    variables[i] = 0xFFFF;
  }

  // Calculate offset at end of page
  unsigned int end_offset = (fs_p->page_size - page_start_offset) % (size + v_addr_size);
  // Last possible address
  uintptr_t next_addr = from_page + fs_p->page_size - end_offset - size - v_addr_size;
  
  // Update page state on receiving page
  fs_mark_page(fs_p, to_page, RECEIVE_DATA);

  // Loop "from_page"
  while(next_addr >= (from_page + page_start_offset)) {
    uint16_t v_addr = 0xFFFF;
    v_addr = *((uint16_t*) (next_addr  + size));
    if(v_addr != 0xFFFF) {
      //log_msg("v_addr: : %x\n", v_addr);
      int status = fs_add_unique_to_array(variables, v_addr, max_num_vars);
      //log_msg("Status: %i\n", status);
      if(status == 0) {
        // Copy to new page
        uintptr_t new_addr = fs_find_free_address(fs_p, to_page, size);
        //log_msg("new_addr offset: %d\n", new_addr - to_page);
        if(new_addr != no_valid_page_e) {
          if(fs_write_variable_int(fs_p, new_addr, (uint8_t*)next_addr, size, v_addr) != 0) {
            error = -1;
            log_msg("fs_move_to_page: unable to write variable to new page!\n");
            break;
          }
          else {
            log_msg("Variable v_addr: %x, successfully written\n", v_addr);
          }
        }
        else {
          error = -2;
          log_msg("fs_move_to_page: unable to find free space on new page!\n");
          break;
        }
      }
    } // if(v_addr != 0xFFFF
    // Set next address: Substract variavle size + size of virtual address
    next_addr -= (size + v_addr_size);
  }
  if(error == 0) {
    // update page markings
    fs_mark_page(fs_p, to_page, VALID_PAGE);
    // Erase previous page
    fs_p->erase(from_page);
  }
  return error;
}

/*
 * @brief Write a variable to a specific memory address
 * note: the memory address needs to be erased and valid, else the function will fail
 */
int fs_write_variable_int(fs_memory_setup_t *fs_p, uintptr_t address, uint8_t *data, uint32_t size, uint16_t virtual_address) {
  int status = 0;

  status = fs_p->blob(address, data, size);
  if(status != 0) {
    log_msg("fs_write_variable: Unable to write blob data\n");
  }

  status = fs_p->write(address + size, virtual_address);
  if(status != 0) {
    log_msg("fs_write_variable: Unable to write virtual address\n");
  }

   return status;
}

/*
 * @brief Find available memory address on page
 * @return valid adress or "no_valid_page_e"
 */
uintptr_t fs_find_free_address(fs_memory_setup_t *fs_p, uintptr_t page, uint32_t size) {
  uintptr_t free_addr = no_valid_page_e;

  uintptr_t end_address = page + fs_p->page_size - page_start_offset;
  uintptr_t address = page + page_start_offset;

  // Loop through page from beginning
  while (address < end_address) {
    // if virtual address is free (v_addr located at end of variable)
    if ((*(volatile uint16_t*)(address + size)) == 0xFFFF) {
      break;
    }
    else {
      address += size + v_addr_size;
    }
  }
  
  if(address <= (end_address - size - v_addr_size)) {
    free_addr = address;
  }
  return free_addr;
}
/*
 * @brief Write a variable to the memory system.
 * The function will:
 * 1. Find a valid page to write on
 * 2. Find valid address
 * 3. If no valid address, change page
 * 4. Write the actual data to the memory system
 */
int fs_write_and_verify_variable(fs_memory_setup_t *fs_p, uint16_t virtual_address, uint8_t *data, uint32_t size) {
  uintptr_t page;
  int error = 0;

  page = fs_get_write_page(fs_p);

  if(page != no_valid_page_e) {
    uintptr_t address;
    
    address = fs_find_free_address(fs_p, page, size);

    // No free space on page, move to other page
    if( address == no_valid_page_e) {
      // If page is full move to new page and consolidate
      if(page == fs_p->page0) {
        // From page 0 to page 1
        fs_move_to_page(fs_p, fs_p->page0, fs_p->page1, size);
        page = fs_p->page1;
        log_msg("fs_write_and_verify_variable: move from 0 to 1\n");
      }
      else {
        // From page 1 to page 0
        fs_move_to_page(fs_p, fs_p->page1, fs_p->page0, size);
        page = fs_p->page0;
        log_msg("fs_write_and_verify_variable: move from 1 to 0\n");
      }

      // Find new address on new page
      address = fs_find_free_address(fs_p, page, size);
    }
    
    // If valid address
    if( address != no_valid_page_e) {
      fs_write_variable_int(fs_p, address, data, size, virtual_address);
    } // if( address <= (end_address - size - 2))
    else {
      error = -2;
      log_msg("fs_write_and_verify_variable: end of MOVED page!\n");
    }
    // ----------------------------------------------------
  }
  else {
    log_msg("fs_write_and_verify_variable: no valid page\n");
    error = -1;
  }

  return error;
}

/*
 *
 */
int fs_init_valid(fs_memory_setup_t *fs_p, uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status) {
  int error = 0;
  // Invalid state -> format eeprom
  if (page_1_status == VALID_PAGE) {

     // Erase both Page0 and Page1 and set Page0 as valid page
     fs_format(fs_p, page_0, page_1);
     //TODO: check error
  }
  else { // if (PageStatus1 == VALID_PAGE)
    // Page0 valid, Page1 erased
    if (page_1_status == ERASED) {

    /* Erase Page1 */
      fs_p->erase(page_1);
    // TODO: Error handling
    }
    else
    { // if (PageStatus1 == ERASED)
      // Page0 valid, Page1 receive

      // TODO: Transfer data from Page0 to Page1

      // Mark Page1 as valid
      fs_mark_page(fs_p, page_1, VALID_PAGE);
      // TODO: error handling

      // Erase Page0
      fs_p->erase(page_0);
    } // if (PageStatus1 == VALID_PAGE)
  }
  return error;
}
/*
 *
 */
int fs_init_receive(fs_memory_setup_t *fs_p, uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status) {
  int error = 0;

  // Page0 receive, Page1 valid
  if (page_1_status == VALID_PAGE) {
    // TODO:
    // Transfer data from Page1 to Page0

    // Mark Page0 as valid
    error = fs_mark_page(fs_p, page_0, VALID_PAGE);
    //TODO: Check error

    // Erase Page1
    error = fs_p->erase(page_1);
    //TODO: Check error

  }
  else { // if (PageStatus1 == VALID_PAGE)

    // Page0 receive, Page1 erased
    if (page_1_status == ERASED) {
      // Erase Page1
      error = fs_p->erase(page_1);
      //TODO: Check error

      // Mark Page0 as valid
      error = fs_mark_page(fs_p, page_0, VALID_PAGE);
      //TODO: Check error
    }
    else { // if (PageStatus1 == ERASED)
      // Invalid state -> format eeprom

      fs_format(fs_p, page_0, page_1);

    } /* if (PageStatus1 == ERASED) */
  } /* if (PageStatus1 == VALID_PAGE) */

  return error;
}

/*
 *
 */
int fs_format(fs_memory_setup_t *fs_p, uintptr_t page_0, uintptr_t page_1) {
  int error;

  // Erase Page0
  error = fs_p->erase(page_0);
  if(error == 0) {
    //log_msg("fs_format programming VALID_PAGE:\n");

    error = fs_mark_page(fs_p, page_0, VALID_PAGE);

    if(error != 0) {
      log_msg("fs_format: Failed to mark page 0\n");
    }

    error = fs_p->erase(page_1);
    if(error != 0) {
      log_msg("fs_format: Failed to erase page 1\n");

    }
  }
  else {
    log_msg("fs_format: Failed to erase page 0\n");
  }
  return error;
}

/*
 *
 */
int fs_init_erased(fs_memory_setup_t *fs_p, uintptr_t page_0, uintptr_t page_1, uint16_t page_0_status, uint16_t page_1_status) {
  int error = 0;

  // Page0 erased, Page1 valid
  if (page_1_status == VALID_PAGE) {

    error = fs_p->erase(page_0);
    if(error != 0) {
      // Failed to erase page 0
      log_msg("fs_init_erase: failed to erase page 0\n");
    }
  }
  else { // if (PageStatus1 == VALID_PAGE)
    // Page0 erased, Page1 receive
    if (page_1_status == RECEIVE_DATA) {
      // Erase Page0
      error = fs_p->erase(page_0);
      if(error == 0) {

        //log_msg("fs_init_erased programming: VALID_PAGE:\n");
        // Mark Page1 as valid
        error = fs_mark_page(fs_p, page_1, VALID_PAGE);

        if(error != 0) {
           log_msg("fs_init_erased: Failed to program flash\n");
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

      log_msg("fs_init_erased: formatting\n");
      // Erase both Page0 and Page1 and set Page0 as valid page
      error = fs_format(fs_p, page_0, page_1);
      if(error != 0) {
        // Failed to format pages
        log_msg("fs_init_erased: Failed to format\n");
      }

    } // if (PageStatus1 == RECEIVE_DATA)
  } // if (PageStatus1 == VALID_PAGE)
  return error;
}

/*
 *
 */

int fs_mark_page(fs_memory_setup_t *fs_p, uintptr_t addr, uint16_t mark) {
  int error;
  // Mark Page
  error = fs_p->write(addr, mark);
  //status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, mark);
  if(error != 0) {
     log_msg("fs_mark_page: Failed to program flash\n");
     error = -1;
   }
  return error;
}
