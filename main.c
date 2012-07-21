/*
 * main.c
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

#define puts(x) lcdPutString(x)

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <twi.h>
#include <time.h>
#include <serial.h>
#include <motor.h>
#include <misc.h>
#include <adc.h>
#include <cam.h>
#include <mem.h>

// Remember: Strings to the lcd should not end with \n
// Timer 0 (8 bit): Servo PWM
// Timer 1 (16bit): Motor Speed PWM
// Timer 2 (8 bit): System Time (Currently unused)
// Timer 3 (16bit): Unused
// Timer 4 (16bit): Unused
// Timer 5 (16bit): Unused

// LED 0 & 1 used as motor display.
// LEd 2 should be free...

char buffer[BUFFERSIZE]; // Used as global string buffer
char versionString[] PROGMEM = "xyRobot BETA\n";

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

uint8_t upDownPos = MIDDLE;
uint8_t leftRightPos = CENTER;

uint8_t bluetoothConnected = 0;
char bluetoothPartner[15];

void printMenu(uint8_t menu);
void menuHandler(void);
void remoteHandler(void);
void sendCamPic(uint8_t fast);
void readBluetoothPartner(void);

int main(void) {

	ledInit();
	ledToggle(2);
	driveInit();
	twiInit();
	lcdInit();
	serialInit(UART_BAUD_SELECT(38400,16000000L), 8, NONE, 1);
	adcInit();
	camInitPorts();
	memInit();
	initSystemTimer();
	sei();
	printMenu(MENU_MAIN); // Print Menu for the first time
	ledToggle(2);

	while(1) {
		menuHandler();
		remoteHandler();

		rotateUpDown(upDownPos);
		rotateLeftRight(leftRightPos);
	}
	return 0;
}

void printMenu(uint8_t menu) {
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
		lcdPutString("\nStarting RAM Test...");
		lcdPutString("Error Rate: ");
		lcdPutString(byteToString(memCalcErrorRate()));
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

/* Serial commands:
 * '?'						--> Send Version String
 * 0x80, x					--> Move Camera Servo Up/Down to x Degree (from 0 to 180)
 * 0x81, x					--> Move Camera Servo Left/Right to x Degree (from 0 to 180)
 * 0x82, r1 ... r8			--> Sends picture. Camera settings registers are r1 to r8
 * 0x83, dist, speed, dir	--> Drive dist cm with speed speed, dir 1 = Forward, 0 = backward
 * 0x84, degree, dir		--> Turn. Dir 1 = right, 0 = left
 * 0x85, r1 ... r8			--> Send picture, but use only 4bit per pixel!
 * 0x86						--> Get distance
 *
 * 'C'...					--> Bluetooth "CONNECT"
 * 'D'...					--> Bluetooth "DISCONNECT"
 */

void remoteHandler() {
	uint8_t c;
	uint8_t temp, temp2, temp3;

	if (serialHasChar()) {
		c = serialGet();
		switch(c) {
		case 'C':
			readBluetoothPartner();
			bluetoothConnected = 1;
			break;

		case 'D':
			serialReadLine();
			bluetoothConnected = 0;
			break;

		case 'E':
			serialReadLine();
			break;

		case 'O':
			serialReadLine();
			break;

		case '?':
			strcpy_P(buffer, versionString);
			serialWriteString(buffer);
			break;

		case 0x80:
			temp = serialGet();
			if (temp <= 180) {
				upDownPos = temp;
			} else {
				upDownPos = MIDDLE;
			}
			break;

		case 0x81:
			temp = serialGet();
			if (temp <= 180) {
				leftRightPos = temp;
			} else {
				leftRightPos = CENTER;
			}
			break;

		case 0x82:
			sendCamPic(0);
			break;

		case 0x83:
			temp = serialGet();
			temp2 = serialGet();
			temp3 = serialGet();
			if (temp3 != 0) {
				temp3 = FORWARD;
			} else {
				temp3 = BACKWARD;
			}
			drive(temp, temp2, temp3);
			break;

		case 0x84:
			temp = serialGet();
			temp2 = serialGet();
			if (temp2 != 0) {
				temp2 = RIGHT;
			} else {
				temp2 = LEFT;
			}
			turn(temp, temp2);
			break;

		case 0x85:
			sendCamPic(1);
			break;

		case 0x86:
			serialWrite(getDistance());
			break;
		}
	}
}

void readBluetoothPartner() {
	uint8_t toRead;
	time_t startTime = getSystemTime();

	toRead = 9; // "ONNECT  '"
	while (toRead > 0) {
		if (serialHasChar()) {
			serialGet();
			toRead--;
		}
		if (diffTime(startTime, getSystemTime()) > 1000) {
			bluetoothPartner[0] = 'T';
			bluetoothPartner[1] = 'M';
			bluetoothPartner[2] = 'T';
			bluetoothPartner[3] = '\0';
			return;
		}
	}

	toRead = 14;
	while (toRead > 0) {
		if (serialHasChar()) {
			bluetoothPartner[14 - toRead] = serialGet();
		}
		if (diffTime(startTime, getSystemTime()) > 1000) {
			bluetoothPartner[0] = 'T';
			bluetoothPartner[1] = 'M';
			bluetoothPartner[2] = 'T';
			bluetoothPartner[3] = '\0';
			return;
		}
	}
	bluetoothPartner[14] = '\0';
	serialReadLineTimeout(1000);
}

void sendCamPic(uint8_t fast) {
	uint16_t i = 0;
	uint8_t reg[8];
	while (i < 8) {
		while (!serialHasChar());
		reg[i++] = serialGet();
	}
	if (fast == 0) {
		camSendSerial(reg, 8);
	} else {
		camSendSerial(reg, 4);
	}
}
