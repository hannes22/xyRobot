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
#include <stdlib.h>
#include <util/delay.h>

#include <misc.h>
#include <twi.h>
#include <serial.h>

// First bit: Background light status
uint8_t lcdStatus;
uint8_t lcdCharsSent = 0;

#define CHARSTOSEND 10
#define TIMETOWAIT 100

extern char buffer[64];

void ledInit() {
    DDRA |= (1 << DDA7) | (1 << DDA6);
	DDRB |= (1 << DDB4);
	PORTA |= (1 << PA7) | (1 << PA6);
	PORTB |= (1 << PB4);
}

void ledFlash() {
	uint8_t i;
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

void lcdInit() {
	lcdStatus = 1; // first bit set: bg light on
	lcdPutChar(12); // Clear display
	lcdPutChar(27); // Command Mode
	lcdPutChar(79); // Set cursor position
	lcdPutChar(1); // x (1...20)
	lcdPutChar(1); // y (1...4)
	lcdPutChar(27); // Command Mode again
	lcdPutChar(123); // Clear key buffer
	lcdSetBackgroundLight(1);
}

void lcdSetBackgroundLight(uint8_t status) {
	lcdPutChar(27);
	lcdPutChar(76);
	if (status == 0) {
		lcdPutChar(0);
		lcdStatus &= ~(1); // reset first bit: bg light off
	} else {
		lcdPutChar(1);
		lcdStatus |= (1); // set first bit
	}
}

void lcdFlashBackgroundLight() {
	// Flashes light, takes 1 second
	lcdSetBackgroundLight(!(lcdStatus & 1));
	_delay_ms(500);
	lcdSetBackgroundLight(!(lcdStatus & 1));
	_delay_ms(500);
}

void lcdPutChar(char c) {
	twiStartWait(LCD_ADDRESS+I2C_WRITE);
	twiWrite(c);
	twiStop();
}

void lcdPutString(char* data) {
	twiStartWait(LCD_ADDRESS+I2C_WRITE);
	while(*data != '\0') {
		lcdCharsSent++;
		if (*data != '\r') {
			if (*data == '\n') {
				twiWrite('\r');
				twiWrite('\n');
				data++;
			} else {
				twiWrite(*data++);
			}
		} else {
			data++;
		}
		_delay_ms(20); // Wait for lcd
	}
	twiStop();
	if (lcdCharsSent >= CHARSTOSEND) {
		lcdCharsSent = 0;
		_delay_ms(TIMETOWAIT);
	}
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
					erg[i++] = wert - '0';
				} else fin = 1;
			} else fin = 1;
		}
	}
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

uint8_t *serialReadLine(void) {
	uint8_t ptr = 0;
	while(1) {
		if (serialHasChar()) {
			buffer[ptr] = serialGet();
			serialWrite(buffer[ptr]);
			if ((buffer[ptr] == '\n') || (ptr == sizeof(buffer) - 1)) {
				buffer[ptr] = '\0';
				return (uint8_t *)buffer;
			}
			if (buffer[ptr] != '\r') {
				// ignore carriage return (\r)
				if (buffer[ptr] == 8) {
					// handle backspace
					ptr--;
				} else {
					ptr++;
				}
			}
		}
	}
}

uint16_t serialReadNumber(uint8_t base) {
	uint8_t *s = serialReadLine();
	uint16_t val = (uint16_t)strtoul((char *)s, NULL, base);
	return val;
}

char *byteToString(uint8_t byte) {
    return bytesToString((uint16_t)byte);
}

char *bytesToString(uint16_t bytes) {
	return utoa(bytes, buffer, 10);
}

char *byteToHex(uint8_t byte) {
	return utoa(byte, buffer, 16);
}
