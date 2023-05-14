/*
 * settings.c
 *
 *  Created on: 20 Feb 2023
 *      Author: ben
 */


#include "settings.h"
#include "stm32f1xx_hal.h"
#include "SEGGER_RTT.h"
#include <stdio.h>
#include "crc.h"
#include "flash_storage.h"

#define NUM_IA (24)
#define NUM_PC (5)

typedef struct __attribute__((packed)){
  ia_t ias[NUM_IA];
  pc_t pcs[NUM_PC];
} settings_t ;

/****************************************
 * Factory Settings
 ***************************************/
 static const settings_t settings_factory = {
  .pcs = {
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 0,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 1,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 2,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 3,
     },
     {
       .mode = PC_MODE_ON,
       .chan = 0,
       .pc = 4,
     },
  },
  .ias = {
      // IA 0-7
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 80,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '0', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_TOGGLE,
        .midi_chan = 10,
        .midi_data0 = 81,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '1', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 82,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '2', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 83,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '3', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 84,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '4', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 85,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '5', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 86,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '6', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 87,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '7', 0},
      },
      // IA 8-15
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 88,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '8', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_TOGGLE,
        .midi_chan = 10,
        .midi_data0 = 89,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '0', '9', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 90,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '1', '0', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 91,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '1', '1', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 92,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '1', '2', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 93,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '1', '3', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 94,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '1', '4', 0},
      },
      {
        .type = IA_TYPE_CC,
        .mode = IA_MODE_LATCH,
        .midi_chan = 10,
        .midi_data0 = 95,
        .midi_data1 = 0x7F,
        .midi_data2 = 0,
        .id = {'I', 'A', '1', '5', 0},
      },
      // IA 8-15
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 96,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '6', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_TOGGLE,
       .midi_chan = 10,
       .midi_data0 = 97,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '7', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 98,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '8', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 99,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '1', '9', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 100,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '0', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 101,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '1', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 102,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '2', 0},
      },
      {
       .type = IA_TYPE_CC,
       .mode = IA_MODE_LATCH,
       .midi_chan = 10,
       .midi_data0 = 103,
       .midi_data1 = 0x7F,
       .midi_data2 = 0,
       .id = {'I', 'A', '2', '3', 0},
      },
  }
};

 /****************************************
  * Local variables
  ***************************************/
static settings_t const *settings_current = 0 ;
static const int settings_preset_mem_offset = 512;

/****************************************
 * Private function declarations
 ***************************************/
static int hal_eeprom_byte_write(uint16_t addr, uint8_t *b);
static int hal_eeprom_byte_read(uint16_t addr, uint8_t *b);
static int hal_eeprom_page_write(uint16_t addr, uint8_t *b, uint8_t size);
static int hal_eeprom_bulk_read(uint16_t addr, uint8_t *b, uint8_t size);
static int settings_preset_nr_to_addr(uint8_t nr);
static void hal_dump_mem(uint8_t *buf, uint8_t size);

static void hal_eeprom_read_back(uint16_t addr, uint8_t *data, uint8_t size) {
  uint8_t read[16] = {0};
  char buf[128];
  int len = 0;

  HAL_Delay(2);
  hal_eeprom_bulk_read(addr, read, size);
  // Print write
  len = 0;
  for(int i = 0; i < size; i++) {
    len += sprintf(&buf[len], "%x,", data[i]);
  }
  buf[len++] = '\n';
  buf[len] = 0;
  SEGGER_RTT_WriteString(0, buf);
  // Print read
  len = 0;
  for(int i = 0; i < size; i++) {
    len += sprintf(&buf[len], "%x,", read[i]);
  }
  buf[len++] = '\n';
  buf[len] = 0;
  SEGGER_RTT_WriteString(0, buf);
  // C0mpare
  for(int i = 0; i < size; i++) {
    if(data[i] != read[i]) {

      SEGGER_RTT_printf(0, "[%d] r:%d != w:%d\n", i, read[i], data[i] );
    }
  }
}
/****************************************
 * Public functions
 ***************************************/
/*
 * @brief Initialize the settings module
 */
void settings_init() {
  //uint8_t write[16] = "Benjamin Jensen."; //['B', 'e', 'n', 'j', 'a', 'm', 'i' 'n', ' ', 'J', 'e']
  // uint8_t write[16] = "Fantasi dyr 2000";
/*
  char buf[128];
  int len = 0;
  len = sprintf(buf, "Settings size: %d\n\r", sizeof(settings_t));
  buf[len] = 0;
  SEGGER_RTT_WriteString(0, buf);
  // Load stores settings
  hal_eeprom_page_write(16, write, 16);
  HAL_Delay(2);
  hal_eeprom_read_back(16, write, 16);
*/
  // validate loaded settings
  // if not valid
  //    Load factory settings
  flash_storage_init();
  settings_current = &settings_factory;
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

  flash_storage_read(addr, (uint8_t*)preset, 10);
  //hal_eeprom_bulk_read(addr, (uint8_t*)preset, 10);
  crc = crc16_calc((uint8_t*)preset + 2, 8);
  SEGGER_RTT_printf(0, "settings_load_preset: nr: %d, addr: %d\n", nr, addr);
  if(crc != preset->crc) {
    error = -1;
    hal_dump_mem((uint8_t*)preset, 10);
    SEGGER_RTT_printf(0, "Load preset(crc error) load: %d != %d\n", preset->crc, crc);
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

  SEGGER_RTT_printf(0, "Write preset\naddr: %d, nr: %d, crc: %x\n", addr, nr, preset->crc);

  flash_storage_write(addr, (uint8_t*)preset, 10);
  //int addr = settings_preset_mem_offset + nr * sizeof(preset);
  //hal_eeprom_page_write(addr, (uint8_t*)preset, 10);
  //hal_eeprom_read_back(addr, (uint8_t*)preset, 10);

  return error;
}
/****************************************
 * Private function declarations
 ***************************************/
static void hal_dump_mem(uint8_t *buf, uint8_t size) {
  for(int i = 0; i < size; i++) {
    SEGGER_RTT_printf(0, "%x, ", buf[i]);
  }
  SEGGER_RTT_printf(0, "\n");
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
/*
 *
 */
static int settings_initialie_i2c(I2C_HandleTypeDef *hi2c_handle) {
  int error = 0;
  /* Peripheral clock enable */
   __HAL_RCC_I2C1_CLK_ENABLE();

   hi2c_handle->Instance = I2C1;
   hi2c_handle->Init.ClockSpeed = 300*1000;
   hi2c_handle->Init.DutyCycle = I2C_DUTYCYCLE_2;
   hi2c_handle->Init.OwnAddress1 = 0;
   hi2c_handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
   hi2c_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
   hi2c_handle->Init.OwnAddress2 = 0;
   hi2c_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
   hi2c_handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
   if (HAL_I2C_Init(hi2c_handle) != HAL_OK)
   {
     error = -1;
     SEGGER_RTT_WriteString(0, "Settings: Unable to initialize I2C!\n");
   }
   return error;
}
/*
 * EEPROM: 24c08
 * Datasheet: https://ww1.microchip.com/downloads/en/devicedoc/21081G.pdf
 * Memory: 4x256x8
 * Layout: 4 blocks of 256 bytes
 * Address (control byte): b[1, 0, 1, 0, B2 B1, B0, R/W]
 * Block address: B2, b1, b0 0x0-0x3 is valid for the 24C08
 * R/W: 0x1 = Read, 0x0 = Write
 * Page write: up to 16 bytes can be written at a time
 *
 * Settings 512 bytes from page 0+1
 * Preset size 8 bytes: 64 presets from Block 2+3
 */
static const uint8_t eeprom_addr = 0xA0;
/*
 * @brief Write a byte to the specified eeprom address
 */
static int hal_eeprom_byte_write(uint16_t addr, uint8_t *b) {
  int error = 0;
  I2C_HandleTypeDef hi2c_handle;
  settings_initialie_i2c(&hi2c_handle);
  uint8_t ctrl = eeprom_addr | 0x0; // Write
  HAL_I2C_Mem_Write(&hi2c_handle, ctrl, 0, 1, b, 1, 10);

  return error;
}
/*
 * @brief Read a byte from the specified eeprom address
 */
static int hal_eeprom_byte_read(uint16_t addr, uint8_t *b) {
  int error = 0;
  uint8_t ctrl = eeprom_addr | 0x1; // Read
  uint8_t actual_addr = 0;
  I2C_HandleTypeDef hi2c_handle;

  settings_initialie_i2c(&hi2c_handle);

  if(addr >= (256*3)) {
    ctrl |= 0x03 << 1;
    actual_addr = addr - 256*3;
  }
  else if(addr >= (256*2)) {
    ctrl |= 0x02 << 1;
    actual_addr = addr - 256*2;
  }
  else if(addr >= 256) {
    ctrl |= 0x01 << 1;
    actual_addr = addr - 256;
  }
  //HAL_I2C_Mem_Read(&hi2c_handle, eeprom_addr | ((addr & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, b, 1, 10);

  return error;
}
/*
 * @brief Write a page (16 bytes) to eeprom
 */
static int hal_eeprom_page_write(uint16_t addr, uint8_t *b, uint8_t size) {
  int error = 0;
  uint8_t ctrl = eeprom_addr | 0x0; // Write
  uint8_t actual_addr = 0;
  I2C_HandleTypeDef hi2c_handle;

  settings_initialie_i2c(&hi2c_handle);
  // Set up block
  if(addr >= (256*3)) {
    ctrl |= 0x03 << 1;
    actual_addr = addr - 256*3;
  }
  else if(addr >= (256*2)) {
    ctrl |= 0x02 << 1;
    actual_addr = addr - 256*2;
  }
  else if(addr >= 256) {
    ctrl |= 0x01 << 1;
    actual_addr = addr - 256;
  }
  SEGGER_RTT_printf(0, "hal_eeprom_page_write: addr: %d, ctrl: %x, size\n", actual_addr, ctrl, size);
  hal_dump_mem(b, size);
  // Check if content can be within one page
  if(size <= 16) {
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c_handle, eeprom_addr | ((addr & 0x0300) >> 7), (addr & 0xff), I2C_MEMADD_SIZE_8BIT, b, size, 10);
    if(status != 0) {
      SEGGER_RTT_printf(0, "hal_eeprom_page_write: error(%d)", status);
    }
  }

  return error;
}
/*
 * @brief Read up to a full content of memory in the eeprom
 *
 */
static int hal_eeprom_bulk_read(uint16_t addr, uint8_t *b, uint8_t size) {

  uint8_t ctrl = eeprom_addr | 0x1; // Read
  uint8_t actual_addr = 0;
  int error = 0;
  I2C_HandleTypeDef hi2c_handle;

  settings_initialie_i2c(&hi2c_handle);

  if(addr >= (256*3)) {
    ctrl |= 0x03 << 1;
    actual_addr = addr - 256*3;
  }
  else if(addr >= (256*2)) {
    ctrl |= 0x02 << 1;
    actual_addr = addr - 256*2;
  }
  else if(addr >= 256) {
    ctrl |= 0x01 << 1;
    actual_addr = addr - 256;
  }
  SEGGER_RTT_printf(0, "hal_eeprom_bulk_read: addr: %d, ctrl: %x\n", actual_addr, ctrl);

  HAL_StatusTypeDef status;
   status = HAL_I2C_Mem_Read(&hi2c_handle, eeprom_addr | ((addr & 0x0300) >> 7), (addr & 0xff), I2C_MEMADD_SIZE_8BIT, b, size, 10);
   if(status != 0) {
     SEGGER_RTT_printf(0, "hal_eeprom_bulk_read: error(%d)", status);
   }

  /*
  HAL_StatusTypeDef status;
  status = HAL_I2C_Mem_Read(&hi2c_handle, ctrl, actual_addr, 1, b, size, 10);
  if(status != 0) {
    SEGGER_RTT_printf(0, "hal_eeprom_bulk_read: error(%d)", status);
  }
  */
  return error;
}
