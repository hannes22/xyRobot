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

uint8_t *camInit(uint8_t *regs) {
    uint8_t *pic;
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
    
#ifdef CAMDEBUG
    DEBUGPUTS("waiting\n");
#endif
    
    camWait();
    
#ifdef CAMDEBUG
    DEBUGPUTS("waited\n");
#endif
    
    pic = (uint8_t *)malloc(128*128);
    for (i = 0; i < (128*128); i++) {
        setClock(1);
        adcStart(CAMOUT);
        _delay_us(CAMDELAY);
        pic[i] = adcGet();
        setClock(0);
        _delay_us(CAMDELAY);
    }
    
#ifdef CAMDEBUG
    DEBUGPUTS("return\n");
#endif
    
    return pic;
}

uint8_t *camShoot(void) {
    uint8_t *pic;
    uint16_t i;
    if (camInitialized == 0) {
        return camInit(NULL);
    } else {
        camStart();
        camWait();
        pic = (uint8_t *)malloc(128*128);
        adcStart(CAMOUT);
        for (i = 0; i < (128*128); i++) {
            while (adcReady() == 0);
            pic[i] = adcGet();
            setClock(1);
            _delay_us(CAMDELAY);
            setClock(0);
            while ((CAMPIN & (1 << CAMREAD)) != 0) {
                setClock(1);
                setClock(0);
            }
        }
        return pic;
    }
}

void camReset(void) {
    camInitialized = 0;
    reset();
}