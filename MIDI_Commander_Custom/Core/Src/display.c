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
#include "display.h"
#include "draw.h"
#include "leds.h"

static uint8_t need_update = 0;
static I2C_HandleTypeDef *hi2c1_;

/****************************************
 * Private functions declarations
 ***************************************/
static void display_test() ;
static void display_prep_line(char *buf, uint8_t line, uint8_t pc);
static void draw_pixel_wrapper(int x, int y);

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
  draw_set_pixel_function(&draw_pixel_wrapper);
  draw_set_fill_function(&ssd1306_Fill);

  //display_test();
  //ssd1306_UpdateScreen(hi2c1);
  disp_preset_t p = {
      .pc = {1,22,33,44,115},
      .bank = 23,
  };
  display_show_preset(&p);
  display_update();
}

/*
 * 1px top and bottom + 10px font = 12 px pr line
 * 12px x 5 = 60px + 2px bottom and top
 */
/*
 * @brief Display Preset "PRESET" state
 */
void display_show_preset(disp_preset_t *preset) {
  char buf[6];

  draw_fill(Black);
  // Write PC numbers to display
  for(int i = 0; i < 5; i++) {
    ssd1306_SetCursor(1, 2 + (i * 12));
    display_prep_line(buf, i, preset->pc[i]);
    ssd1306_WriteString(buf, Font_7x10, White);
  }

  // Vertical separator line
  for(int i = 0; i < 64; i++)
    ssd1306_DrawPixel(38, i, White);

  // Write bank display
  if(preset->bank < 10) {
    draw_char('0', 42, 0, &font_robot56);
    draw_char('0' + preset->bank, 83, 0, &font_robot56);
  }
  else {
    sprintf(buf, "%d", preset->bank);
    draw_char(buf[0] , 42, 0, &font_robot56);
    draw_char(buf[1] , 83, 0, &font_robot56);
  }
  need_update = 1;

  // Update LEDS
  led_all_off();
  if(preset->leds & DISP_LED_P0) {
    leds_set_led(5, LED_ON);
  }
  else if(preset->leds & DISP_LED_P1) {
    leds_set_led(6, LED_ON);
  }
  else if(preset->leds & DISP_LED_P2) {
    leds_set_led(7, LED_ON);
  }
  else if(preset->leds & DISP_LED_P3) {
    leds_set_led(8, LED_ON);
  }
  if(preset->leds & DISP_LED_IA0) {
      leds_set_led(1, LED_ON);
  }
  if(preset->leds & DISP_LED_IA1) {
    leds_set_led(2, LED_ON);
  }
  if(preset->leds & DISP_LED_IA2) {
    leds_set_led(3, LED_ON);
  }

}
/*
 * @brief Display Preset "BANK" state
 */
void display_bank_display(int bank) {
  char buf[4];
  draw_fill(Black);
 // 43 * 2 = 86
 // 128 - 86 = 42 / 2 = 21
  // char0=21, char1=21+43=54
  // Write bank display

  if(bank < 10) {
    draw_char('0', 21, 0, &font_robot56);
    draw_char('0' + bank, 53, 0, &font_robot56);
  }
  else {
    sprintf(buf, "%d", bank);
    draw_char(buf[0] , 21, 0, &font_robot56);
    draw_char(buf[1] , 53, 0, &font_robot56);
  }
  need_update = 1;

  // Update LEDS
  led_all_off();
}
/*
 * @brief Display Preset "IAx" state
 */
static const int tag1_x = 0;
static const int tag2_x = 28 + 5;
static const int tag3_x = 28 + 5 + 28 + 5;
static const int tag4_x = 28 + 5 + 28 + 5 + 28 + 5;
static const int tag_l1_y = 0;
static const int tag_l2_y = 63-9;
static const int vline1_x = 31;
static const int vline2_x = 31+28+5;
static const int vline3_x = 31+28+5+28+5;
void display_iax_display(disp_iax_t *data) {

  draw_fill(Black);

  // Draw top section
  draw_string(data->ias[0].id ,tag1_x , tag_l1_y, &font_robot10);
  draw_string(data->ias[1].id ,tag2_x , tag_l1_y, &font_robot10);
  draw_string(data->ias[2].id ,tag3_x , tag_l1_y, &font_robot10);
  draw_string(data->ias[3].id ,tag4_x , tag_l1_y, &font_robot10);
  draw_vline(vline1_x, tag_l1_y, 10);
  draw_vline(vline2_x, tag_l1_y, 10);
  draw_vline(vline3_x, tag_l1_y, 10);
  draw_hline(10,0,127);

  // Draw center section
  ssd1306_SetCursor(0, 10+9);
  ssd1306_WriteString(data->name, Font_16x26, White);

  // Draw bottom section
  draw_hline(63-10,0,127);
  draw_string(data->ias[4].id ,tag1_x ,tag_l2_y , &font_robot10);
  draw_string(data->ias[5].id ,tag2_x ,tag_l2_y , &font_robot10);
  draw_string(data->ias[6].id ,tag3_x ,tag_l2_y , &font_robot10);
  draw_string(data->ias[7].id ,tag4_x ,tag_l2_y , &font_robot10);

  draw_vline(vline1_x,tag_l2_y, 63);
  draw_vline(vline2_x,tag_l2_y, 63);
  draw_vline(vline3_x,tag_l2_y, 63);

  need_update = 1;

  // update LEDS
  led_all_off();
  if(data->leds & 0x01) {
    leds_set_led(1, LED_ON);
  }
  if(data->leds & 0x02) {
    leds_set_led(2, LED_ON);
  }
  if(data->leds & 0x04) {
    leds_set_led(3, LED_ON);
  }
  if(data->leds & 0x08) {
    leds_set_led(4, LED_ON);
  }
  // IA 4-7
  if(data->leds & 0x10) {
    leds_set_led(6, LED_ON);
  }
  if(data->leds & 0x20) {
    leds_set_led(7, LED_ON);
  }
  if(data->leds & 0x40) {
    leds_set_led(8, LED_ON);
  }
  if(data->leds & 0x80) {
    leds_set_led(9, LED_ON);
  }
}
/****************************************
 * Private functions
 ***************************************/
/*
 *
 */
static void draw_pixel_wrapper(int x, int y) {
  ssd1306_DrawPixel(x, y, White);
}
/*
 *
 */
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
