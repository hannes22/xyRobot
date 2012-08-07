/*
 * serialHandler.c
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

/* Serial commands:
 * See Readme.md for the commands used by xyRobotRemote.
 *
 * 'C'...					--> Bluetooth "CONNECT"
 * 'D'...					--> Bluetooth "DISCONNECT"
 * 'O'...					--> Bluetooth "OK"
 * 'E'...					--> Bluetooth "ERROR"
 * 'R'...					--> Read until '\n' then reset (comfortable bootloader usage)
 */
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#include <serialHandler.h>
#include <serial.h>
#include <motor.h>
#include <cam.h>

uint8_t bluetoothConnected = 0;
char bluetoothPartner[15];
extern char versionString[] PROGMEM; // Define in main!
extern char buffer[BUFFERSIZE]; // Used as global string buffer, defined in main
extern uint8_t upDownPos; // Global servo position registers. Defined in main
extern uint8_t leftRightPos;

void readBluetoothPartner(void);
void sendCamPic(uint8_t depth);

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
			serialReadLineTimeout(250);
			bluetoothConnected = 0;
			break;

		case 'E':
			serialReadLineTimeout(250);
			break;

		case 'O':
			serialReadLineTimeout(250);
			break;

		case 'R':
			serialReadLineTimeout(250);
			wdt_enable(WDTO_15MS);
			while(1);
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
			rotateUpDown(upDownPos);
			break;

		case 0x81:
			temp = serialGet();
			if (temp <= 180) {
				leftRightPos = temp;
			} else {
				leftRightPos = CENTER;
			}
			rotateLeftRight(leftRightPos);
			break;

		case 0x82:
			sendCamPic(8);
			break;

		case 0x85:
			sendCamPic(4);
			break;

		case 0x87:
			sendCamPic(2);
			break;

		case 0x88:
			sendCamPic(1);
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

void sendCamPic(uint8_t depth) {
	uint8_t i = 0;
	uint8_t reg[8];
	while (i < 8) {
		while (!serialHasChar());
		reg[i++] = serialGet();
	}

	camStore(reg, 31);
	camSendStored(31, depth);
}
