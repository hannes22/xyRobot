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

#define DEBUGPUTS(x) lcdPutString(x)

#ifdef CAMDEBUG
#include "include/misc.h"
#endif

uint8_t camInitialized = 0;

void setClock(uint8_t load) {
    // _delay_us(CAMDELAY);
    if (load == 1) {
        CAMPORT |= (1 << CAMCLOCK);
    }
    
    if (load == 0) {
        CAMPORT &= ~(1 << CAMCLOCK);
    }
    
    if (load == 2) {
        CAMPORT ^= (1 << CAMCLOCK);
    }
    // _delay_us(CAMDELAY);
}

void reset(void) {
    CAMPORT &= ~(1 << CAMRESET);
    setClock(1);
    _delay_us(CAMDELAY);
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
    _delay_us(CAMDELAY);
    setClock(0);
    CAMPORT &= ~(1 << CAMLOAD);
}

void setRegisters(uint8_t *regs) {
    // regs: array 8 bytes
    uint8_t reg, i, a = 1;
    for (reg = 0; reg < 8; reg++) {
        
        sendBit(a & 4, 0);
        sendBit(a & 2, 0);
        sendBit(a & 1, 0);
        
        for (i = 0; i < 8; i++) {
            sendBit(regs[i] & (1 << (7 - i)), (i == 7));
        }
        
        if (a < 7) {
            a++;
        } else {
            a = 0;
        }
    }
}

void camStart(void) {
    CAMPORT |= (1 << CAMSTART);
    setClock(1);
    _delay_us(CAMDELAY);
    CAMPORT &= ~(1 << CAMSTART);
    setClock(0);
}

void camWait(void) {
    while ((CAMPIN & (1 << CAMREAD)) == 0) {
        setClock(1);
        _delay_us(CAMDELAY);
        if ((CAMPIN & (1 << CAMREAD)) != 0) {
            break;
        }
        setClock(0);
        _delay_us(CAMDELAY);
    }
}

void camInit(uint8_t *regs) {
	// Initialize camera with given settings register (8 byte array)
    uint16_t i;
    
    CAMDDR |= (1 << CAMSTART) | (1 << CAMDATA) | (1 << CAMLOAD) | (1 << CAMRESET) | (1 << CAMCLOCK);
    CAMDDR &= ~(1 << CAMREAD);
    
    camInitialized = 1;

#ifdef CAMDEBUG
    DEBUGPUTS("camInit()\n");
#endif
    reset();
    
    if (regs != NULL) {
        setRegisters(regs);
    } else {
        regs = (uint8_t *)malloc(8);
        regs[0] = 16;
        regs[1] = 0;
        regs[2] = 90;
        regs[3] = 1;
        regs[4] = 0;
        regs[5] = 1;
        regs[6] = 0;
        regs[7] = 191;
        
        setRegisters(regs);
        free(regs);
        regs = NULL;
    }
    
#ifdef CAMDEBUG
    DEBUGPUTS("regs set\n");
#endif
    
    camStart();
    
#ifdef CAMDEBUG
    DEBUGPUTS("cam started\n");
#endif
    
    camWait();
    
#ifdef CAMDEBUG
    DEBUGPUTS("waited\n");
#endif
    
    for (i = 0; i < (128*128); i++) {
        setClock(1);
        _delay_us(CAMDELAY);
        setClock(0);
        _delay_us(CAMDELAY);
    }

#ifdef CAMDEBUG
    DEBUGPUTS("return\n");
#endif
}

void camShoot(void) {
	// Start shooting a picture
    uint16_t i;
    if (camInitialized == 0) {
        camInit(NULL);
    }
    camStart();
    camWait();
    adcStart(CAMOUT);
}

uint8_t camGetByte(void) {
	// Get one byte of the picture shot.
	// Shoots a picture if not already done
	uint8_t result;
	if (camInitialized == 0) {
		camShoot();
	}
	while(adcReady() == 0);
	result = adcGet();
	setClock(1);
	_delay_us(CAMDELAY);
	setClock(0);
	while ((CAMPIN & (1 << CAMREAD)) != 0) {
	    setClock(1);
	    setClock(0);
	}
	return result;
}

void camReset(void) {
	// Reset camera, so you can load other settings
    camInitialized = 0;
    reset();
    adcClose();
}
