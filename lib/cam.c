/*
 * cam.c
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

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include <adc.h>
#include <cam.h>
#include <misc.h>

#define CAMDELAY 1

void camShoot(void);

void setClock(uint8_t load) {
	_delay_us(2);
	if (load == 1) {
		CAMPORT |= (1 << CAMCLOCK);
	}
	
	if (load == 0) {
		CAMPORT &= ~(1 << CAMCLOCK);
	}
	
	if (load == 2) {
		CAMPORT ^= (1 << CAMCLOCK);
	}
}

void reset(void) {
	CAMPORT &= ~(1 << CAMRESET);
	setClock(1);
	CAMPORT |= (1 << CAMRESET);
	setClock(0);
}

void sendBit(uint8_t bit, uint8_t load) {
	if (bit != 0) {
		CAMPORT |= (1 << CAMDATA);
	} else {
		CAMPORT &= ~(1 << CAMDATA);
	}
	setClock(1);
	if (load != 0) {
		CAMPORT |= (1 << CAMLOAD);
	}
	setClock(0);
	CAMPORT &= ~(1 << CAMLOAD);
}

void setRegisters(uint8_t *regs) {
	// regs: array 8 bytes
	uint8_t reg, i;
	for (reg = 0; reg < 8; reg++) {
		
		sendBit(reg & 4, 0);
		sendBit(reg & 2, 0);
		sendBit(reg & 1, 0);
		
		for (i = 0; i < 8; i++) {
			sendBit(regs[reg] & (1 << (7 - i)), (i == 7));
		}
	}
}

void camStart(void) {
	CAMPORT |= (1 << CAMSTART);
	setClock(1);
	CAMPORT &= ~(1 << CAMSTART);
	setClock(0);
}

void camWait(void) {
	while ((CAMPIN & (1 << CAMREAD)) == 0) {
		setClock(1);
		setClock(0);
	}
}

uint8_t camCanRead(void) {
	return ((CAMPIN & (1 << CAMREAD)) != 0);
}

void camInitPorts(void) {
	CAMDDR |= (1 << CAMSTART) | (1 << CAMDATA) | (1 << CAMLOAD) | (1 << CAMRESET) | (1 << CAMCLOCK);
	CAMDDR &= ~(1 << CAMREAD);
	CAMPORT &= ~((1 << CAMCLOCK) | (1 << CAMDATA) | (1 << CAMSTART) | (1 << CAMLOAD));
	CAMPORT |= (1 << CAMRESET);
}

void camInit(uint8_t *regs) {
	// Initialize camera with given settings register (8 byte array). Automatically shoots a picture

	camInitPorts();
	reset();
	
	if (regs != NULL) {
		setRegisters(regs);
	} else {
		regs = (uint8_t *)malloc(8);
		regs[0] = 0x7F; // Z & Offset
		regs[1] = 0x02; // N & VH & Gain
		regs[2] = 0x00; // Exposure 2
		regs[3] = 90; // Exposure 1
		regs[4] = 0x01; // P
		regs[5] = 0x00; // M
		regs[6] = 0x01; // X
		regs[7] = 0x04; // E & V
		
		setRegisters(regs);
		free(regs);
		regs = NULL;
	}
	
	camShoot();
}

void camShoot(void) {
	// Start shooting a picture
	camStart();
	camWait();
}

uint8_t camGetByte(void) {
	// Get one byte of the picture shot.
	// Shoots a picture if not already done
	uint8_t result;

	if (!camCanRead()) {
		camShoot();
	}

	setClock(1);
	setClock(0);
	adcStart(CAMOUT);
	while(adcReady() == 0);
	result = adcGet(0);

	return result;
}

void camReset(void) {
	// Reset camera, so you can load other settings
	reset();
}
