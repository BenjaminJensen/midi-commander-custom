/*
 * display.c
 *
 *  Created on: 8 Jul 2021
 *      Author: D Harvie
 */
#include "main.h"
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "eeprom_midi_settings.h"


void display_init(void){
    ssd1306_Init();

    ssd1306_SetCursor(5,0);

    ssd1306_WriteString("*CUSTOM*", Font_11x18, White);
    ssd1306_SetCursor(5, 22);
    ssd1306_WriteString(FIRMWARE_VERSION, Font_7x10, White);

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


    ssd1306_SetCursor(30,20);
    for(int i=0; i<4; i++){
    	ssd1306_WriteChar((char)*pString++, Font_11x18, White);
    }

    ssd1306_SetCursor(30,40);
    for(int i=0; i<8; i++){
    	ssd1306_WriteChar((char)*pString++, Font_7x10, White);
    }

    ssd1306_UpdateScreen();
}
