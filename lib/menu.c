/*
 * menu.c
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

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <misc.h>
#include <motor.h>
#include <cam.h>
#include <serial.h>

/*
 * This menu can handle more than 10 Menu Entries (including the header),
 * but it is not very pretty... Be advised!
 */

void menu(void);
void menuDriving(void);
void menuCamera(void);
void menuBluetooth(void);
void putPicSerial(void);
void putPicLcd(void);
void menuServo(void);

#define MENUMSGS 5
// 3 entries per page
// Number 1 starts with second entry
// If you reach 9, continue with 1 again, insert empty line
// Start with \n
// page 1
char messageA[] PROGMEM = "\nxyRobot Beta";
char messageB[] PROGMEM = "\n1) Driving";
char messageC[] PROGMEM = "\n2) Camera";
// page 2
char messageD[] PROGMEM = "\n3) Bluetooth";
char messageE[] PROGMEM = "\n4) Servos";

PGM_P menuMessages[MENUMSGS] PROGMEM = { messageA, messageB, messageC, messageD, messageE };

char buffer[64];

char nextPageString[] PROGMEM = "\n0) Next page";
char directionString[] PROGMEM = "\nDirection: 2 4 6 8";
char forwardString[] PROGMEM = "\nForwards";
char leftString[] PROGMEM = "\nLeft";
char rightString[] PROGMEM = "\nRight";
char backwardString[] PROGMEM = "\nBackwards";
char distanceString[] PROGMEM = "\nDistance? ";
char degreeString[] PROGMEM = "\nDegrees? ";
char servoString[] PROGMEM = "\nServo Control\n1)Up-Down\n2)Left-Right\n0)Exit";
char upDownString[] PROGMEM = "\nU-D? ";
char leftRightString[] PROGMEM = "\nL-R? ";
char camInitString[] PROGMEM = "\nInitializing Camera";
char initString[] PROGMEM = "\nInitialized";
char camString[] PROGMEM = "\nxyRob Gameboy Camera";
char sendBTString[] PROGMEM = "1: Send Bluetooth";
char printString[] PROGMEM = "\n2: Print here";
char sendString[] PROGMEM = "\nSending...";

// Function pointers for messages. entry 0, 10, 20... NULL because it is not selectable
void (*menuFunctions[MENUMSGS])(
		void) = {NULL, &menuDriving, &menuCamera, &menuBluetooth, &menuServo };

void menu(void) {
	uint8_t temp;
	uint8_t i = 0;
	uint16_t c;
	uint8_t page = 0; // a page has 3 entries
	uint8_t pageChanged = 1;

	while (1) {
		// print entries
		if (pageChanged != 0) {
			if ((page * 3) < MENUMSGS) {
				strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuMessages[page * 3])));
				lcdPutString(buffer);
				_delay_ms(200); // Wait for lcd
			}
			if (((page * 3) + 1) < MENUMSGS) {
				strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuMessages[(page * 3) + 1])));
				lcdPutString(buffer);
				_delay_ms(200); // Wait for lcd
			} else {
				lcdPutString("\n");
			}
			if (((page * 3) + 2) < MENUMSGS) {
				strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuMessages[(page * 3) + 2])));
				lcdPutString(buffer);
				_delay_ms(200); // Wait for lcd
			} else {
				lcdPutString("\n");
			}
			strcpy_P(buffer, nextPageString);
			lcdPutString(buffer);
			_delay_ms(200); // Wait for lcd
		}

		// get input from LCD, react if it exists
		c = lcdGetChar();
		if (c == '0') {
			// Next page requested
			if ((MENUMSGS / 3) > page) { // There are more entries
				page++;
			} else {
				page = 0;
			}
			pageChanged = 1;
		} else if ((c != '?') && (c != '*') && (c != '#') && (c != 0)) {
			c -= '0';
			if (((page * 3) + 2) > 9) {
				// the number the user has to enter is smaller
				temp = (page * 3) + 2;
				while (temp > 9) {
					temp -= 10;
					i++;
				}
				c += (i * 10);
			}
			if (c < MENUMSGS)
				if (menuFunctions[c] != NULL)
					(*menuFunctions[c])();
		}
	}
}

void menuDriving(void) {
	uint8_t c;
	uint8_t dir;
	uint16_t dist;

	while (1) {

		hell:
		strcpy_P(buffer, directionString);
		lcdPutString(buffer);
		_delay_ms(100); // Wait for lcd
		while ((c = lcdGetChar()) == 0)
			;
		switch (c - '0') {
		case 2:
			strcpy_P(buffer, forwardString);
			lcdPutString(buffer);
			dir = FORWARD;
			break;
		case 4:
			strcpy_P(buffer, leftString);
			lcdPutString(buffer);
			dir = LEFT;
			break;
		case 6:
			strcpy_P(buffer, rightString);
			lcdPutString(buffer);
			dir = RIGHT;
			break;
		case 8:
			strcpy_P(buffer, backwardString);
			lcdPutString(buffer);
			dir = BACKWARD;
			break;
		case 0:
			return;

		default:
			goto hell;
		}
		_delay_ms(100); // Wait for lcd
		if ((dir == RIGHT) || (dir == LEFT)) {
			strcpy_P(buffer, degreeString);
			lcdPutString(buffer);
		} else {
			strcpy_P(buffer, distanceString);
			lcdPutString(buffer);
		}
		_delay_ms(100); // Wait for lcd
		dist = lcdGetNum();
		lcdPutString(bytesToString(dist));

		if ((dir == RIGHT) || (dir == LEFT)) {
			turn(dist, dir);
		} else {
			drive(dist, 200, dir);
		}
	}
}

void menuServo() {
	uint8_t b;
	uint16_t c;

	while(1) {
		strcpy_P(buffer, servoString);
		lcdPutString(buffer);

		_delay_ms(500); //Wait for lcd
		while ((b = lcdGetChar()) == 0);
		switch (b - '0') {
		case 0:
			return;
		case 1:
			strcpy_P(buffer, upDownString);
			lcdPutString(buffer);
			_delay_ms(100); //Wait for lcd
			while ((c = lcdGetNum()) > 255);
			lcdPutString(bytesToString(c));
			rotateUpDown((uint8_t)c);
			_delay_ms(1000); //Wait for user to see
			break;
		case 2:
			strcpy_P(buffer, leftRightString);
			lcdPutString(buffer);
			_delay_ms(100); //Wait for lcd
			while ((c = lcdGetNum()) > 255);
			lcdPutString(bytesToString(c));
			rotateLeftRight((uint8_t)c);
			_delay_ms(1000); //Wait for user to see
			break;
		}
	}
}

void menuCamera(void) {
	uint8_t c;
	strcpy_P(buffer, camInitString);
	lcdPutString(buffer);
	camInit(NULL);
	strcpy_P(buffer, initString);
	lcdPutString(buffer);
	_delay_ms(100); // Wait for lcd
	while(1) {
		strcpy_P(buffer, camString);
		lcdPutString(buffer);
		_delay_ms(100); // Wait for lcd
		strcpy_P(buffer, sendBTString);
		lcdPutString(buffer);
		_delay_ms(100); // Wait for lcd
		strcpy_P(buffer, printString);
		lcdPutString(buffer);
		_delay_ms(100); // Wait for lcd
		while ((c = lcdGetChar()) == 0);
		c -= '0';
		switch(c) {
		case 1:
			camShoot();
			strcpy_P(buffer, sendString);
			lcdPutString(buffer);
			putPicSerial();
			break;
		case 2:
			camShoot();
			putPicLcd();
			_delay_ms(1000);
			break;
		case 0:
			return;
		}
	}
}

void putPicSerial() {
	uint16_t i;
	for (i = 0; i < (128 * 128); i++) {
		serialWrite(camGetByte());
	}
}

void putPicLcd() {
	uint16_t i;
	uint8_t c = 0;
	lcdPutString("\n");
	for (i = 0; i < (128 * 128); i++) {
		if ((i % 4) == 0) {
			lcdPutChar(c + 32);
			if (c < 94) {
				c++;
			} else {
				c = 0;
			}
		}
		lcdPutString("0x");
		lcdPutString(byteToHex(camGetByte()));
		if ((i % 4) != 3) {
			lcdPutChar(' ');
		} else {
			_delay_ms(50); // Wait for lcd
		}
	}
}

void menuBluetooth(void) {
	while (1) {
		lcdPutString("\nBluetooth...");
		if (lcdGetChar() != 0) {
			break;
		}
	}
}
