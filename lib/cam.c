//
//  cam.c
//  Robot
//
//  Created by Thomas Buck on 28.07.11.
//  Copyright 2011 Claude-Dornier-Schule. All rights reserved.
//

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "include/adc.h"
#include "include/cam.h"

void camPulseClock(uint8_t load) {
    if (load == 2) {
        CAMPORT &= ~(1 << CAMRESET);
    }
    if (load == 3) {
        CAMPORT |= (1 << CAMSTART);
    }
    CAMPORT |= (1 << CAMCLOCK);
    if (load == 1) {
        CAMPORT |= (1 << CAMLOAD);
    }
    _delay_us(CAMDELAY);
    if (load == 2) {
        CAMPORT |= (1 << CAMRESET);
    }
    if (load == 3) {
        CAMPORT &= ~(1 << CAMSTART);
    }
    CAMPORT &= ~(1 << CAMCLOCK);
    if (load == 1) {
        CAMPORT &= ~(1 << CAMLOAD);
    }
    _delay_us(CAMDELAY);
}

void camPushByte(uint8_t b, uint8_t a) {
    uint8_t j;
    for (j = 4; j > 0; j /= 2) {
        if ((a & j) == 0) {
            CAMPORT &= ~(1 << CAMDATA);
        } else {
            CAMPORT |= (1 << CAMDATA);
        }
        camPulseClock(0);
    }
    for (j = 128; j > 0; j /= 2) {
        if ((b & j) == 0) {
            CAMPORT &= ~(1 << CAMDATA);
        } else {
            CAMPORT |= (1 << CAMDATA);
        }
        if (j == 1) {
            camPulseClock(1);
        } else {
            camPulseClock(0);
        }
    }
}

void camInit(uint8_t *regs) {
    uint8_t i, a = 0;
    CAMDDR |= (1 << CAMSTART) | (1 << CAMDATA) | (1 << CAMLOAD) | (1 << CAMRESET) | (1 << CAMCLOCK);
    CAMDDR &= ~(1 << CAMREAD);
    camPulseClock(2);
    for (i = 0; i < 8; i++) {
        if (a < 7) {
            a++;
        } else {
            a = 0;
        }
        camPushByte(regs[i], a);
    }
}

void camShoot(void) {
    camPulseClock(3);
}

uint8_t camReady(void) {
    CAMPORT |= (1 << CAMCLOCK);
    if ((CAMPIN & (1 << CAMREAD)) == 0) {
        CAMPORT &= ~(1 << CAMCLOCK);
        _delay_us(CAMDELAY);
        return 0;
    } else {
        CAMPORT &= ~(1 << CAMCLOCK);
        _delay_us(CAMDELAY);
        return 1;
    }
}

uint8_t *camGetPicture(void) {
    uint16_t i; // 16384 pixels to read...
    uint8_t *pic;
    if (camReady() == 0) {
        return NULL;
    }
    pic = (uint8_t *)malloc(128 * 128);
    for (i = 0; i < 16384; i++) {
        pic[i] = 0;
    }
    adcInit(CAMOUT);
    i = 0;
    while ((CAMPIN & (1 << CAMREAD)) != 0) {
        camPulseClock(0);
        while (adcReady() == 0);
        pic[i++] = adcGet();
    }
    adcClose();
    return pic;
}