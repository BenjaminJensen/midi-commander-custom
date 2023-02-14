/*
 * display.c
 *
 *  Created on: 8 Jul 2021
 *      Author: D Harvie
 */
#include "main.h"
#include "flash_midi_settings.h"
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"


void display_init(void){
    ssd1306_Init();

    ssd1306_SetCursor(5, 22);
    ssd1306_WriteString(FIRMWARE_VERSION, Font_7x10, White);

    // A little animation


    for(int i=0; i < 11; i++){
    	ssd1306_SetCursor(i*11,0);
		ssd1306_WriteString("#", Font_11x18, White);
		ssd1306_SetCursor(i*11,46);
		ssd1306_WriteString("#", Font_11x18, White);

		ssd1306_UpdateScreen();
		HAL_Delay(50);
    }

    __NOP();
}

void display_disp_button(uint16_t b) {
	const int loop = 12;
	char s[loop];
	s[0] = 'b';
	s[1]= ' ';
	for(int i = 2; i < (2 + 10); i++) {
		uint8_t state = (b & (1 << ( i - 2) ) ) ? 1: 0;
		s[i] = '0' + state;
	}
	ssd1306_SetCursor(10, 0);
	for(int i = 0; i < (loop - 5); i++){
		ssd1306_WriteChar(s[i], Font_11x18, White);
	}
	// 34 + 18(font heigh) + 9 (half line)
	ssd1306_SetCursor(32, 18+5);
	for(int i = 7; i < loop; i++){
		ssd1306_WriteChar(s[i], Font_11x18, White);
	}

	ssd1306_UpdateScreen();
}

void display_setConfigName(void){
    ssd1306_SetCursor(10, 34);
    for(int i=0; i<16; i++){
    	ssd1306_WriteChar(pGlobalSettings[16+i], Font_7x10, White);
    }
    ssd1306_UpdateScreen();
}


void display_setBankName(uint8_t bankNumber){
	uint8_t *pString = pBankStrings + (12 * bankNumber);

	ssd1306_Fill(Black);

	const char* string = "1234";

    ssd1306_SetCursor(30,20);
    for(int i=0; i<4; i++){
    	ssd1306_WriteChar(string[i], Font_11x18, White);
    }

    ssd1306_SetCursor(30,40);
    for(int i=0; i<8; i++){
    	ssd1306_WriteChar((char)*pString++, Font_7x10, White);
    }

    ssd1306_UpdateScreen();
}
