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
#include <serial.h>
#include <motor.h>
#include <misc.h>
#include <adc.h>
#include <cam.h>
#include <time.h>

// Remember: Strings to the lcd should not end with \n
// Timer 0 (8 bit): Motor --> Servos
// Timer 1 (16bit): Motor_Low
// Timer 2 (8 bit): SystemTime
// Timer 3 (16bit): Unused
// Timer 4 (16bit): Unused
// Timer 5 (16bit): Unused

// LED 0 & 1 used as motor display.
// LEd 2 should be free...

char buffer[128]; // Used as global string buffer
char versionString[] PROGMEM = "xyRobot BETA\n";

#define MENUMSGS 5
char messageA[] PROGMEM = "\nxyRobot Beta";
char messageB[] PROGMEM = "\n1) Driving";
char messageC[] PROGMEM = "\n2) Camera";
char messageD[] PROGMEM = "\n3) Bluetooth";
char messageE[] PROGMEM = "\n4) Servos";
PGM_P menuMessages[MENUMSGS] PROGMEM = { messageA, messageB, messageC, messageD, messageE };

char nextPageString[] PROGMEM = "\n0) Next page";

// Function pointers for messages. Heading is not selectable
void (*menuFunctions[MENUMSGS])(void) = { NULL, NULL, NULL, NULL, NULL };

uint8_t page = 0;

void printMenu(void);
void menuHandler(void);
void remoteHandler(void);
void sendCamPic(void);

int main(void) {

	uint64_t time;

	ledInit();
	ledToggle(0);
	ledToggle(1);
	ledToggle(2);
	driveInit();
	_delay_ms(250);
	
	twiInit();
	lcdInit();
	serialInit(UART_BAUD_SELECT(38400,16000000L), 8, NONE, 1);
	adcInit();
	initSystemTimer();
	ledToggle(0);
	ledToggle(1);
	ledToggle(2);

	sei();

	printMenu(); // Print Menu for the first time

	serialWriteString("Initialized!\n");

	while(1) {

		menuHandler();
		remoteHandler();

		/* time = getSystemTime();
		if ((time % 1000) == 0) {} */
	}
	return 0;
}

void printMenu() {
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
}

void menuHandler() {
	uint16_t c = lcdGetChar();
	uint8_t temp, i = 0;
	if (c == '0') {
		if ((MENUMSGS / 3) > page) { // There are more entries
			page++;
		} else {
			page = 0;
		}
		printMenu();
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
			if (menuFunctions[c] != NULL) {
				(*menuFunctions[c])();
			}
		}
	}
}

/* Serial commands:
 *
 * '?'						--> Send Version String
 * 0x80, x					--> Move Camera Servo Up/Down to x Degree (from 0 to 180)
 * 0x81, x					--> Move Camera Servo Left/Right to x Degree (from 0 to 180)
 * 0x82, r1 ... r8			--> Sends picture. Camera settings registers are r1 to r8
 * 0x83, dist, speed, dir	--> Drive dist cm with speed speed, dir 1 = Forward, 0 = backward
 * 0x84, degree, dir		--> Turn. Dir 1 = right, 0 = left
 *
 * default					--> Send revieced character back
 */

uint8_t upDownPos = UD_CENTER;
uint8_t leftRightPos = LR_CENTER;

void remoteHandler() {
	uint8_t c;
	uint8_t temp, temp2, temp3;

	if (serialHasChar()) {
		c = serialGet();
		switch(c) {
		case '?':
			strcpy_P(buffer, versionString);
			serialWriteString(buffer);
			break;

		case 0x80:
			temp = serialGet();
			if (temp <= 180) {
				rotateUpDown(temp);
			}
			break;

		case 0x81:
			temp = serialGet();
			if (temp <= 180) {
				rotateLeftRight(temp);
			}
			break;

		case 0x82:
			sendCamPic();
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

		default:
			serialWrite(c);
			break;
		}
	}
}

void sendCamPic() {
	uint16_t i = 0;
	uint8_t reg[8];
	while (i < 8) {
		while (!serialHasChar());
		reg[i++] = serialGet();
	}

	camInit(reg);
	// Transmit all the bytes...
	for (i = 0; i < 16384; i++) {
		serialWrite(camGetByte());
	}
	camReset();
}
