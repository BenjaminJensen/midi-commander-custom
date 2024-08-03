/*
 * midi.c
 *
 *  Created on: Feb 19, 2023
 *      Author: ben
 */

#include "midi.h"
#include "uart.h"

void midi_send_cc(uint8_t chan, uint8_t cc, uint8_t val) {

  uart_putc(0xB0 | (chan & 0x0F)); // CC chan
  uart_putc(cc & 0x7F); // CC nr
  uart_putc(val & 0x7F); // CC value
}

/*
 * http://midi.teragonaudio.com/tech/midispec/pgm.htm
 */
void midi_send_pc(uint8_t chan, uint8_t nr) {
  uart_putc(0xC0 | (chan & 0x0F)); // PC chan
  uart_putc(nr & 0x7F); // PC nr
}
