/*
 * main.c
 *
 *  Created on: 06.07.2011
 *      Author: thomas
 */

#define puts(x) serialWriteString(x)

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "include/serial.h"
#include "include/twi.h"
#include "include/misc.h"
#include "include/motor.h"
#include "include/cam.h"

int main(void) {

	uint16_t i, j = 0;

    uint8_t *pic;
    
    uint8_t camConf[8] = { 0, 0, 90, 1, 0, 1, 2, 128 };
    
	twiInit();
	ledInit();
	serialInit(51, 8, NONE, 1);
	motorInit();
    
	sei();
    
    puts("Initializing Camera...\n");
    camInit(camConf);
    puts("Initialized!\n");
    
while(1) {
    puts("Shooting Picture...\n");
    camShoot();
    puts("Waiting for Camera...\n");
    while (camReady() == 0);
    puts("Getting Picture...\n");
    pic = camGetPicture();
    puts("Here it goes:\n");
    for (i = 0; i < 16384; i++) {
        serialWriteString("0x");
        serialWriteString(byteToString(pic[i]));
        serialWrite(' ');
        if (++j >= 5) {
            j = 0;
            serialWrite('\n');
            _delay_ms(100);
        }
    }
    _delay_ms(10000);
}
	return 0;
}
