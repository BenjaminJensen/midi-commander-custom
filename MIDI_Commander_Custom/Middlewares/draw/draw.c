/*
 * draw.c
 *
 *  Created on: 5 Mar 2023
 *      Author: ben
 */


#include "draw.h"
#include <stdio.h>
#include "fonts/fontlibrary.h"
#include "SEGGER_RTT.h"

#define DRAW_DEBUG

static draw_pixel_f draw_pixel = 0;
static draw_fill_f draw_fill_ = 0;

/****************************************
 * Private functions declarations
 ***************************************/
#ifdef DRAW_DEBUG
static void send_int(int i, char c);
#endif

/****************************************
 * Public functions
 ***************************************/
/*
 *
 */
int draw_fill(draw_color_e color) {
  if(draw_fill_ != 0) {
    draw_fill_(color);
  }

  return 0;
}
/*
 *
 */
void draw_set_pixel_function(draw_pixel_f f) {
  draw_pixel = f;
}
/*
 *
 */

void draw_set_fill_function(draw_fill_f f) {
  draw_fill_ = f;
}

void draw_vline(int x, int y1, int y2) {
  for(int i = y1; i <= y2; i++) {
    draw_pixel(x, i);
  }
}

void draw_hline(int y, int x1, int x2) {
  for(int i = x1; i <= x2; i++) {
    draw_pixel(i, y);
  }
}

int draw_string(const char *s, int x, int y, fontStyle_t *f) {
  int x_ = x;
  while(*s) {
    draw_char(*s, x_, y, f);
    x_ += f->GlyphWidth[*s- f->FirstAsciiCode];
    s++;
  }
  return 0;
}
/*
 * @brief Draw a character to screen buffer
 */
int draw_char(char c, int x, int y, fontStyle_t *f) {
#ifdef DRAW_DEBUG
  char bufc;
  SEGGER_RTT_WriteString(0, "Draw character\n");
#endif
  if(draw_pixel == 0) {
#ifdef DRAW_DEBUG
  SEGGER_RTT_WriteString(0, "Draw function NULL\n");
#endif
    return -1;
  }
  send_int(x, 'x');
  if(c >= f->FirstAsciiCode && c <= (f->FirstAsciiCode + f->GlyphCount)) {
    //
    int index = (c - f->FirstAsciiCode)*f->GlyphBytesWidth*f->GlyphHeight;
#ifdef DRAW_DEBUG
    send_int(index, 'i');
#endif
    // Outer loop height
    for(int h = 0; h < f->GlyphHeight; h++) {
      // Inner loop width
      for(int w = 0; w < f->GlyphBytesWidth; w++) {
        char pixel = 0;
        // Inner inner loop byte
        for(int b = 0; b < 8; b++) {
          int i = index + h * f->GlyphBytesWidth + w;
          pixel = (f->GlyphBitmaps[i] & (0x80 >> b)) != 0 ? 1: 0;
          if(pixel != 0) {
            if(draw_pixel != 0)
              draw_pixel(w*8 + b + x, h + y);
          }
#ifdef DRAW_DEBUG
          bufc = '0'+pixel;
          SEGGER_RTT_Write(0, &bufc, 1);
#endif
        } // Inner loop byte
      }// Inner loop width
#ifdef DRAW_DEBUG
      bufc = '\n';
      SEGGER_RTT_Write(0, &bufc, 1);
#endif
    }// Outer loop height
  }// If

  return 0;
}

/****************************************
 * Private functions
 ***************************************/
#ifdef DRAW_DEBUG
static void send_int(int i, char c) {
  char buf[8];
  buf[0] = c;
  buf[1] = ':';
  int size = sprintf(&buf[2], "%d", i);
  buf[size+2] = '\n';
  buf[size+3] = 0;
  SEGGER_RTT_WriteString(0, buf);
}
#endif
