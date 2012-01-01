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

#include <misc.h>
#include <motor.h>
#include <cam.h>
#include <serial.h>

/*
 * This menu certainly handles more than 10 Menu Entries (including the header),
 * but it is not very pretty... Be advised!
 */

void menu(void);
void menuDriving(void);
void menuCamera(void);
void menuBluetooth(void);
void putPicSerial(void);
void putPicLcd(void);

#define MENUMSGS 4
// 3 entries per page
// Number 1 starts with second entry
// If you reach 9, continue with 1 again, insert empty line
// Start with \n
// page 1
char messageA[] = "\nxyRobot Beta";
char messageB[] = "\n1) Driving";
char messageC[] = "\n2) Camera";
// page 2
char messageD[] = "\n3) Bluetooth";

char *menuMessages[MENUMSGS] = { messageA, messageB, messageC, messageD };

// Function pointers for messages. entry 0, 10, 20... NULL because it is not selectable
void (*menuFunctions[MENUMSGS])(
		void) = {NULL, &menuDriving, &menuCamera, &menuBluetooth };

void menu(void) {
	uint8_t temp;
	uint8_t i = 0;
	uint16_t c;
	uint8_t page = 0; // a page has 3 entries

	while (1) {
		// print entries
		if ((page * 3) < MENUMSGS) {
			lcdPutString(menuMessages[page * 3]);
			_delay_ms(150); // Wait for lcd
		}
		if (((page * 3) + 1) < MENUMSGS) {
			lcdPutString(menuMessages[(page * 3) + 1]);
			_delay_ms(150); // Wait for lcd
		} else {
			lcdPutString("\n");
		}
		if (((page * 3) + 2) < MENUMSGS) {
			lcdPutString(menuMessages[(page * 3) + 2]);
			_delay_ms(150); // Wait for lcd
		} else {
			lcdPutString("\n");
		}
		lcdPutString("\n0) Next page"); // Goes to page 0 if last page

		_delay_ms(100); // Wait for lcd

		// wait for input, react accordingly
		while ((c = lcdGetChar()) == 0);

		if (c == '0') {
			// Next page requested
			if ((MENUMSGS / 3) > page) { // There are more entries
				page++;
			} else {
				page = 0;
			}
		} else if ((c != '?') && (c != '*') && (c != '#')) {
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

		hell: lcdPutString("\nDirection: 2 4 6 8");
		_delay_ms(100); // Wait for lcd
		while ((c = lcdGetChar()) == 0)
			;
		switch (c - '0') {
		case 2:
			lcdPutString("\nForwards");
			dir = FORWARD;
			break;
		case 4:
			lcdPutString("\nLeft");
			dir = LEFT;
			break;
		case 6:
			lcdPutString("\nRight");
			dir = RIGHT;
			break;
		case 8:
			lcdPutString("\nBackwards");
			dir = BACKWARD;
			break;
		case 0:
			return;

		default:
			goto hell;
		}
		_delay_ms(100); // Wait for lcd
		if ((dir == RIGHT) || (dir == LEFT)) {
			lcdPutString("\nDegrees? ");
		} else {
			lcdPutString("\nDistance? ");
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

void menuCamera(void) {
	uint8_t c;
	lcdPutString("\nInitializing camera");
	camInit(NULL);
	lcdPutString("\nInitialized");
	_delay_ms(100); // Wait for lcd
	while(1) {
		lcdPutString("\nxyRob Gameboy Camera");
		_delay_ms(100); // Wait for lcd
		lcdPutString("1: Send Bluetooth");
		_delay_ms(100); // Wait for lcd
		lcdPutString("\n2: Print here");
		_delay_ms(100); // Wait for lcd
		while ((c = lcdGetChar()) == 0);
		c -= '0';
		switch(c) {
		case 1:
			camShoot();
			lcdPutString("\nSending...");
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
