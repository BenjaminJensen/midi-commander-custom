/*
 * flash_storage.h
 *
 *  Created on: 1 Apr 2023
 *      Author: ben
 */

#ifndef INC_FLASH_STORAGE_H_
#define INC_FLASH_STORAGE_H_

#if __cplusplus
extern "C" {
#endif

#include <stdint.h>

/****************************************
 * Public types for use with the flash libery
 ***************************************/
// Function pointers for HW injection
/*
 * @param flash address to be written
 * @param 16 bit value to be written
 */
typedef int (*flash_write_short)(uintptr_t, uint16_t);
/*
 * @param flash start address to be written
 * @param pointer to 8bit data blob to be written
 * @param size of data to be written
 */
typedef int (*flash_write_blob)(uintptr_t, uint8_t*, uint32_t);
/*
 * @param flash address on page to erase
 */
typedef int (*flash_erase_page)(uintptr_t);
/*
 * @param start address to read from
 * @param pointer to 8 bit buffer to read into
 * @param size of data to be read
 */
typedef int (*flash_read)(uintptr_t, uint8_t*, uint32_t);

//
enum fs_storage_mode_e {INVALID = 0xFF, PAGE0_VALID, PAGE1_VALID, PAGE0_RECEIVING, PAGE1_RECEIVING, PAGE0_MARK_VALID, PAGE1_MARK_VALID};

// Main handle for the libery
typedef struct {
    uint32_t page_size;
    uintptr_t storage_start;
    uintptr_t page0;
    uintptr_t page1;
    uint32_t var_size;
    flash_write_short write;
    flash_read read;
    flash_write_blob blob;
    flash_erase_page erase;
} fs_memory_setup_t;

/****************************************
 * Public functions 
 ***************************************/
int fs_initialize_page(fs_memory_setup_t *fs_p);
int fs_read_variable(fs_memory_setup_t *fs_p, uint16_t v_addr, uint8_t *data);
int fs_write_variable(fs_memory_setup_t *fs_p, uint16_t v_addr, uint8_t *data);

/****************************************
 * Internal Public functions (public for testing purpose)
 ***************************************/
// Private types (public for testing purpose)

fs_memory_setup_t* fs_setup_memory(uintptr_t flash_start, flash_write_short single, flash_write_blob blob, flash_erase_page erase);

int fs_init_erased(fs_memory_setup_t *fs_p, uintptr_t page_0, uintptr_t page_1, uint16_t page_0_status, uint16_t page_1_status);
int fs_init_valid(fs_memory_setup_t *fs_p, uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status);
int fs_init_receive(fs_memory_setup_t *fs_p, uint32_t page_0, uint32_t page_1, uint16_t page_0_status, uint16_t page_1_status);
int fs_mark_page(fs_memory_setup_t *fs_p, uintptr_t addr, uint16_t mark);

int fs_format(fs_memory_setup_t *fs_p, uintptr_t page_0, uintptr_t page_1);

int fs_write_and_verify_variable(fs_memory_setup_t *fs_p, uint16_t virtual_address, uint8_t *data, uint32_t size);
uintptr_t fs_get_read_page(fs_memory_setup_t *fs_p);
uintptr_t fs_get_write_page(fs_memory_setup_t *fs_p);
uintptr_t fs_find_free_address(fs_memory_setup_t *fs_p, uintptr_t page, uint32_t size);
int fs_move_to_page(fs_memory_setup_t *fs_p, uintptr_t from_page, uintptr_t to_page, uint32_t size);
int fs_write_variable_int(fs_memory_setup_t *fs_p, uintptr_t address, uint8_t *data, uint32_t size, uint16_t virtual_address);

uintptr_t fs_locate_variable(fs_memory_setup_t *fs_p, uintptr_t page, uint16_t v_addr, uint32_t size) ;
int fs_add_unique_to_array(uint16_t * array, uint16_t value, uint16_t array_size);
enum fs_storage_mode_e fs_get_storage_mode(fs_memory_setup_t *fs_p);

#if __cplusplus
}
#endif

#endif /* INC_FLASH_STORAGE_H_ */
