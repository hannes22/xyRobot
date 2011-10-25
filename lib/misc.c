/*
 * misc.c
 *
 * Copyright 2011 Thomas Buck <xythobuz@me.com>
 *
 * This file is part of xyRobot.
 *
 * xyRobot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xyRobot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xyRobot.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

#include "include/misc.h"
#include "include/twi.h"
#include "include/serial.h"

char string[4];

void ledInit() {
	int i;
    
    DDRA |= (1 << DDA7) | (1 << DDA6);
	DDRB |= (1 << DDB4);
	PORTA |= (1 << PA7) | (1 << PA6);
	PORTB |= (1 << PB4);
    
    for (i = 0; i < 4; i++) {
        ledToggle(0);
        ledToggle(1);
        ledToggle(2);
        if (i != 3) {
            _delay_ms(333);
        }
    }
}

void ledToggle(uint8_t id) {
	switch (id) {
	case 0:
		PORTA ^= (1 << PA7);
		break;
	case 1:
		PORTA ^= (1 << PA6);
		break;
	case 2:
		PORTB ^= (1 << PB4);
		break;
	}
}

void ledSet(uint8_t id, uint8_t val) {
	if (val != 0) {
		switch (id) {
		case 0:
			PORTA &= ~(1 << PA7);
			break;
		case 1:
			PORTA &= ~(1 << PA6);
			break;
		case 2:
			PORTB &= ~(1 << PB4);
			break;
		}
	} else {
		switch (id) {
		case 0:
			PORTA |= (1 << PA7);
			break;
		case 1:
			PORTA |= (1 << PA6);
			break;
		case 2:
			PORTB |= (1 << PB4);
			break;
		}
	}
}

void lcdPutChar(char c) {
	twiStartWait(LCD_ADDRESS+I2C_WRITE);
	twiWrite(c);
	twiStop();
}

void lcdPutString(char* data) {
	twiStartWait(LCD_ADDRESS+I2C_WRITE);
	while(*data != '\0') {
		if (*data != '\r') {
			if (*data == '\n') {
				twiWrite(13);
				twiWrite(10);
				data++;
			} else {
				twiWrite(*data++);
			}
		} else {
			data++;
		}
	}
	twiStop();
}

uint8_t lcdGetChar(void) {
	uint8_t ret;
	twiStart(LCD_ADDRESS+I2C_READ);
	ret = twiReadAck();
	twiReadNak();
	twiStop();
	return ret;
}

uint16_t lcdGetNum(void) {
	uint8_t wert;
	uint8_t fin = 0;
	uint8_t erg[4];
	uint8_t i = 0;
	uint16_t ret = 0;
	while (fin == 0) {
		wert = lcdGetChar();
		if (wert != 0) {
			if ( (wert != '*') && (wert != '#') ) {
				if (i <= 4) {
					lcdPutChar(wert);
					wert = wert - '0';
					erg[i] = wert;
					i++;
				} else fin = 1;
			} else fin = 1;
		}
	}
	lcdPutString("\n");
	switch (i) {
	case 1:	ret = erg[0];
			break;
	case 2: ret = (10 * erg[0]) + erg[1];
			break;
	case 3: ret = (100 * erg[0]) + (10 * erg[1]) + erg[2];
			break;
	case 4: ret = (1000 * erg[0]) + (100 * erg[1]) + (10 * erg[2]) + erg[3];
			break;
	default:
			break;
	}
	return ret;
}

char *byteToString(uint8_t byte) {
    if (byte < 10) {
        string[0] = byte % 10;
        string[0] += '0';
        string[1] = '\0';
        return string;
    } else if (byte < 100) {
        string[0] = byte / 10;
        string[1] = byte % 10;
        string[0] += '0';
        string[1] += '0';
        string[2] = '\0';
        return string;
    } else {
        string[0] = byte / 100;
        string[1] = byte / 10;
        string[2] = byte % 10;
        string[0] += '0';
        string[1] += '0';
        string[2] += '0';
        string[3] = '\0';
        return string;
    }
}

