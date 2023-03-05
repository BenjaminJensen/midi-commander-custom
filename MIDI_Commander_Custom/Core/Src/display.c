/*
 * display.c
 *
 *  Created on:
 *      Author: Ben
 */
#include "main.h"
#include "flash_midi_settings.h"
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "sh1106.h"
#include "display.h"
static uint8_t need_update = 0;
static I2C_HandleTypeDef *hi2c1_;

static void display_test() ;

static void display_prep_line(char *buf, uint8_t line, uint8_t pc);



/****************************************
 * Public functions
 ***************************************/
/*
 *
 */
void display_update() {
  if(need_update == 1) {
    ssd1306_UpdateScreen(hi2c1_);
    need_update = 0;
  }
}
/*
 *
 */
void display_init(I2C_HandleTypeDef *hi2c1){
  hi2c1_ = hi2c1;
    ssd1306_Init(hi2c1);

    //display_test();
    //ssd1306_UpdateScreen(hi2c1);
    disp_preset_t p = {
        .pc = {1,22,33,44,115},
        .bank = 0,
    };
    display_show_preset(&p);
    display_update();
}

/*
 * 1px top and bottom + 10px font = 12 px pr line
 * 12px x 5 = 60px + 2px bottom and top
 */
void display_show_preset(disp_preset_t *preset) {
  char buf[6];

  for(int i = 0; i < 5; i++) {
    ssd1306_SetCursor(1, 2 + (i * 12));
    display_prep_line(buf, i, preset->pc[i]);
    ssd1306_WriteString(buf, Font_7x10, White);
  }
  // Vertical seperator line
  for(int i = 0; i < 64; i++)
    ssd1306_DrawPixel(38, i, White);

  buf[0] = '0';
  buf[1] = '0';
  buf[2] = 0;

  ssd1306_SetCursor(40, (64-26)/2);
  //ssd1306_WriteString(buf, Font_16x26, White);

  draw_char('0', 42, 0);
  draw_char('1', 83, 0);
  need_update = 1;
}

/****************************************
 * Private functions
 ***************************************/
static void display_prep_line(char *buf, uint8_t line, uint8_t pc) {
  int indent = 0;
  buf[indent++] = '0' + line;
  buf[indent++] = ':';
  if(pc < 100) {
    buf[indent++] = ' ';
  }
  if(pc < 10) {
    buf[indent++] = ' ';
  }
  indent += sprintf(&(buf[indent]),"%d", pc);
  buf[indent] = 0;
}
// Draw a pixel in every corner to verify alignment of display buffer
static void display_test() {
  ssd1306_Fill(Black);
  ssd1306_DrawPixel(0, 0, White);

  ssd1306_DrawPixel(127, 0, White);

  ssd1306_DrawPixel(0, 63, White);

  ssd1306_DrawPixel(127, 63, White);
  need_update = 1;
}
