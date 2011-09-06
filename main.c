/*
 * main.c
 *
 *  Created on: 06.07.2011
 *      Author: thomas
 */

#define puts(x) lcdPutString(x)

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
#include "include/adc.h"

int main(void) {

	uint16_t i;

    uint8_t *pic;
    uint8_t c;
    uint8_t camConf[8] = { 0x0E, 0x06, 0x00, 0x01, 0x00, 0x01, 0x07, 0x80 };
    
	twiInit();
	ledInit();
	serialInit(51, 8, NONE, 1);
	// motorInit();
    adcInit();
    
	sei();
    
    _delay_ms(2000);
    pic = camInit(camConf);
    for (i = 0; i < 16384; i++) {
        lcdPutChar(pic[i]);
        _delay_ms(10);
    }
    free(pic);
    
	while(1) {
        if (serialHasChar()) {
            c = serialGet();
            
            if (c == 'c') {
                pic = camInit(camConf);
                for (i = 0; i < 16384; i++) {
                    serialWrite(pic[i]);
                }
                free(pic);
            }
            
            if (c == '?') {
                serialWriteString("xyRobot BETA\n");
            }
        }
        
	}

	return 0;
}
