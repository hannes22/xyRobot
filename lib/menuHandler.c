/*
 * menuHandler.c
 *
 * Copyright 2012 Thomas Buck <xythobuz@me.com>
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
#include <avr/pgmspace.h>

#include <menuHandler.h>
#include <motor.h>
#include <serialHandler.h> // For Bluetooth Connection State
#include <mem.h> // For Testing SRAM
#include <misc.h> // For talking to LCD
#include <serial.h> // For talking to BTM-222

#define MENUMSGS 6
#define MENU_MAIN 0
#define MENU_BT 1
#define MENU_RAM 2
#define MENU_DRIVE 3
#define MENU_SERVO 4
#define MENU_CAM 5

char messageA[] PROGMEM = "\nxyRobot Beta";
char messageB[] PROGMEM = "\n1) Bluetooth";
char messageC[] PROGMEM = "\n2) SRAM";
char messageD[] PROGMEM = "\n3) Driving";
char messageE[] PROGMEM = "\n4) Servos";
char messageF[] PROGMEM = "\n5) Camera";
PGM_P menuMessages[MENUMSGS] PROGMEM = { messageA, messageB, messageC, messageD, messageE, messageF };
char nextPageString[] PROGMEM = "\n0) Next page";
uint8_t page = 0, menu = MENU_MAIN;

extern char buffer[BUFFERSIZE]; // Global string buffer, defined in main.
extern uint8_t upDownPos;
extern uint8_t leftRightPos; // Global servo position state, defined in main.

void printMenu(uint8_t menu);

void menuInit() {
	printMenu(MENU_MAIN); // Print Menu for the first time
}

void printMenu(uint8_t menu) {
	uint8_t tmp;

	if (menu == MENU_MAIN) {
		if ((page * 3) < MENUMSGS) {
			strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuMessages[page * 3])));
			lcdPutString(buffer);
		}
		if (((page * 3) + 1) < MENUMSGS) {
			strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuMessages[(page * 3) + 1])));
			lcdPutString(buffer);
		} else {
			lcdPutString("\n");
		}
		if (((page * 3) + 2) < MENUMSGS) {
			strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuMessages[(page * 3) + 2])));
			lcdPutString(buffer);
		} else {
			lcdPutString("\n");
		}
		strcpy_P(buffer, nextPageString);
		lcdPutString(buffer);
	} else if (menu == MENU_DRIVE) {
		// Driving
		lcdPutString("\n1) Forwards\n2) Backwards\n3) Turn\n0) Main Menu");
	} else if (menu == MENU_CAM) {
		// Camera
		lcdPutString("\n1) Store pic\n2) Send pic\n\n0) Main Menu");
	} else if (menu == MENU_BT) {
		// Bluetooth
		if (bluetoothConnected) {
			lcdPutString("\nCon: ");
			lcdPutString(bluetoothPartner);
			lcdPutString("\n\n\n0) Main Menu");
		} else {
			lcdPutString("\nBluetooth Ready!");

			lcdPutString("\nName: ");
			serialWriteString("ATN?\r");
			serialReadLineTimeout(150); // "Name\r\n"
			lcdPutString(buffer);
			serialReadLineTimeout(150); // "OK\r\n"

			lcdPutString("\nPin: ");
			serialWriteString("\rATP?\r");
			serialReadLineTimeout(150); // "Pin\r\n"
			lcdPutString(buffer);
			serialReadLineTimeout(150); // "OK\r\n"

			lcdPutString("\n0) Main Menu");
		}
	} else if (menu == MENU_SERVO) {
		// Servos
		lcdPutString("\n1) Left-Right\n3) Up-Down\n\n0) Main Menu");
	} else if (menu == MENU_RAM) {
		// SRAM
		lcdPutString("\n\n\n\nStarting RAM Test...");
		tmp = memCalcErrorRate();
		lcdPutString("Error Rate: ");
		lcdPutString(byteToString(tmp));
		lcdPutString(" ");
		tmp = memCheckErrorRateAgain();
		lcdPutString(byteToString(tmp));
		lcdPutString("%");
		lcdPutString("\n1) Try Again\n0) Main Menu");
	}
}

void menuHandler() {
	uint16_t c = lcdGetChar();
	uint8_t temp, i = 0;
	if (menu == MENU_MAIN) {
		if (c == '0') {
			if (MENUMSGS > ((page + 1) * 3)) { // There are more entries
				page++;
			} else {
				page = 0;
			}
			printMenu(menu);
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
			if (c < MENUMSGS) {
				menu = c;
				printMenu(menu);
			}
		}
	} else if (menu == MENU_DRIVE) {
		// Driving
		if (c == '0') {
			menu = MENU_MAIN;
			printMenu(menu);
		} else if ((c == '1') || (c == '2')) {
			lcdPutString("\nDistance?");
			temp = lcdGetNum();
			lcdPutString("\nSpeed?");
			if (c == '1') {
				drive(temp, (uint8_t)lcdGetNum(), FORWARD);
			} else {
				drive(temp, (uint8_t)lcdGetNum(), BACKWARD);
			}
			printMenu(menu);
		} else if (c == '3') {
			lcdPutString("\nDegrees?");
			c = lcdGetNum();
			lcdPutString("\n1=Left  2=Right");
			temp = lcdGetNum();
			if (temp == 1) {
				turn(c, LEFT);
			} else {
				turn(c, RIGHT);
			}
			printMenu(menu);
		}
	} else if (menu == MENU_CAM) {
		// Camera
		if (c == '0') {
			menu = MENU_MAIN;
			printMenu(menu);
		}
	} else if (menu == MENU_BT) {
		// Bluetooth
		if (c == '0') {
			menu = MENU_MAIN;
			printMenu(menu);
		}
	} else if (menu == MENU_SERVO) {
		// Servos
		if (c == '0') {
			menu = MENU_MAIN;
			printMenu(menu);
		} else if (c == '1') {
			leftRightPos = lcdGetNum();
		} else if (c == '3') {
			upDownPos = lcdGetNum();
		} else if (c == '2') {
			// Up
			if (upDownPos <= (180 - SERVOSTEPWIDTH)) {
				upDownPos += SERVOSTEPWIDTH;
			}
		} else if (c == '8') {
			// Down
			if (upDownPos >= SERVOSTEPWIDTH) {
				upDownPos -= SERVOSTEPWIDTH;
			}
		} else if (c == '4') {
			// Left
			if (leftRightPos >= SERVOSTEPWIDTH) {
				leftRightPos -= SERVOSTEPWIDTH;
			}
		} else if (c == '6') {
			// Right
			if (leftRightPos <= (180 - SERVOSTEPWIDTH)) {
				leftRightPos += SERVOSTEPWIDTH;
			}
		} else if (c == '5') {
			leftRightPos = CENTER;
			upDownPos = MIDDLE;
		}
	} else if (menu == MENU_RAM) {
		// Ram
		if (c == '0') {
			menu = MENU_MAIN;
			printMenu(menu);
		} else if (c != 0) {
			printMenu(menu); // Run test again
		}
	}
}
