/*
 * misc.c
 *
 *  Created on: 12.07.2011
 *      Author: thomas
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "include/misc.h"
#include "include/twi.h"
#include "include/serial.h"

char hexString[3];

void ledInit() {
	DDRA |= (1 << DDA7) | (1 << DDA6);
	DDRB |= (1 << DDB4);
	PORTA |= (1 << PA7) | (1 << PA6);
	PORTB |= (1 << PB4);
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

uint8_t serialGetLine(uint8_t block) {
	uint16_t i = 0;
	uint8_t c = 0;
	uint8_t bedingung = 1;
	while (bedingung) {
		if (block == 0) {
			bedingung = serialHasChar();
			block = 2;
			if (bedingung == 0) {
				return 3;
			}
		}
		if (serialHasChar()) {
			c = serialGet();
		} else {
			if (block == 2) {
				return 3;
			} else {
				continue;
			}
		}
		if (i >= LINE_BUFFER_LENGTH) {
			lineBuffer[LINE_BUFFER_LENGTH - 1] = '\0';
			return 1;
		}
		if (c == '\n') {
			lineBuffer[i] = '\0';
			return 0;
		}
		lineBuffer[i] = c;
		i++;
	}
	if (i >= LINE_BUFFER_LENGTH) {
		lineBuffer[LINE_BUFFER_LENGTH - 1] = '\0';
		return 2;
	} else {
		lineBuffer[i] = '\0';
	}
	return 0;
}

char *byteToString(uint8_t byte) {
    hexString[2] = '\0';
    hexString[1] = byte % 16;
    byte /= 16;
    hexString[0] = byte;
    if (hexString[1] < 10) {
        hexString[1] += '0';
    } else {
        hexString[1] -= 10;
        hexString[1] += 'A';
    }
    if (hexString[2] < 10) {
        hexString[2] += '0';
    } else {
        hexString[2] -= 10;
        hexString[2] += 'A';
    }
    return hexString;
}

