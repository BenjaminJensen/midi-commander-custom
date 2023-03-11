/*
 * draw.h
 *
 *  Created on: 5 Mar 2023
 *      Author: ben
 */

#ifndef INC_DRAW_H_
#define INC_DRAW_H_
#include "fonts/fontlibrary.h"
//
//  Enumeration for screen colors
//
typedef enum {
    Black = 0x00,   // Black color, no pixel
    White = 0x01,   // Pixel is set. Color depends on LCD
} draw_color_e;

typedef void (*draw_pixel_f)(int x, int y);
typedef void (*draw_fill_f)(int color);
void draw_set_pixel_function(draw_pixel_f f);
void draw_set_fill_function(draw_fill_f f);
int draw_char(char c, int x, int y, fontStyle_t *f);
int draw_string(const char *s, int x, int y, fontStyle_t *f);
int draw_fill(draw_color_e color);

void draw_vline(int x, int y1, int y2);
void draw_hline(int y, int x1, int x2);

#endif /* INC_DRAW_H_ */
