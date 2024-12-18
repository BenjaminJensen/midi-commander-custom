
//
// Bitmap font C header file generated by bmfont2c.py 
// 

#ifndef FONTLIBRARY_H_
#define FONTLIBRARY_H_

#include <stdint.h>

typedef struct
{
    uint8_t GlyphCount;
    uint8_t FirstAsciiCode;
    uint8_t GlyphBytesWidth;
    uint8_t GlyphHeight;
    uint8_t FixedWidth;
    uint8_t const *GlyphWidth;
    uint8_t const *GlyphBitmaps;
} fontStyle_t;


extern fontStyle_t font_robot56;
extern fontStyle_t font_robot10;

#endif /* FONTLIBRARY_H_ */
