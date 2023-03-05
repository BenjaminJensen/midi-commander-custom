/*
 * draw.h
 *
 *  Created on: 5 Mar 2023
 *      Author: ben
 */

#ifndef INC_DRAW_H_
#define INC_DRAW_H_


typedef void (*font_draw_pixel_f)(int x, int y);
void draw_set_pixel_function(font_draw_pixel_f f);
int draw_char(char c, int x, int y);

#endif /* INC_DRAW_H_ */
