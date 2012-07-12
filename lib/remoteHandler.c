/*
 * remoteHandler.c
 *
 *  Created on: 03.01.2012
 *      Author: thomas
 */
#include <stdint.h>
#include <stdlib.h>
#include <remoteHandler.h>
#include <serial.h>
#include <cam.h>
#include <avr/pgmspace.h>

char buffer[32];
char versionString[] PROGMEM = "xyRobot BETA\n";

void sendCamPic(void);

/* Serial commands
 *
 * 0x80, 0x## --> Move Camera Servo Up/Down to ## Degree (from 0 to 180)
 * 0x81, 0x## --> Move Camera Servo Left/Right to ## Degree (from 0 to 180)
 * 0x82, 0x##1, ... 0x##8 --> Sends picture. Camera settings registers are 0x##1 to 0x##8
 *
 */

void remoteHandler() {
	uint8_t c;
	if (serialHasChar()) {
		c = serialGet();
		switch(c) {
		case '?':
			strcpy_P(buffer, versionString);
			serialWriteString(buffer);
			break;

		case 'c':
			sendCamPic();

		case 0x80:
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
