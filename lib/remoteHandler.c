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

void sendCamPic(void);

void remoteHandler() {
	uint8_t c;
	if (serialHasChar()) {
		c = serialGet();
		switch(c) {
		case '?':
			serialWriteString("xyRobot BETA\n");
			break;

		case 'c':
			sendCamPic();
			break;

		default:
			serialWrite(c);
			break;
		}
	}
}

void sendCamPic() {
	uint16_t i = 0;
	camInit(NULL);
	// Transmit all the bytes...
	for (i = 0; i < 16384; i++) {
		serialWrite(camGetByte());
	}
	camReset();
}
