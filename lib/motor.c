/*
 * motor.c
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

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <motor.h>
#include <motor_low.h>
#include <misc.h>

#ifdef CMPERTICK
#ifdef TICKSPERCM
#error "motor.c:Define only one constant"
#endif
#endif

#ifndef CMPERTICK
#ifndef TICKSPERCM
#error "motor.c:Define CMPERTICK or TICKSPERCM"
#endif
#endif

volatile uint8_t counterA = 0;
volatile uint8_t cntA = 0;
volatile uint8_t counterB = 0;
volatile uint8_t cntB = 0;

void driveInit() {
    motorInit();
    rotateInit();
    rotateLeftRight(CENTER);
    rotateUpDown(CENTER);
}

ISR(TIMER0_COMPA_vect) {
	// Up down
	// Fake another prescaler divisor of 2
	if (cntA < 100) {
		cntA++;
		return;
	} else {
		cntA = 0;
	}
	switch (counterA) {
	case 0:
		// Enable pin
		UPDOWNPORT |= (1 << UPDOWNSERVO);
		ledSet(2, 0);
		counterA++;
		break;
	case 1:
		// Disable pin
		UPDOWNPORT &= ~(1 << UPDOWNSERVO);
		ledSet(2, 1);
		counterA++;
		break;
	case 2: case 3:
		// OCR0A = 254;
		counterA++;
		break;
	case 4:
		counterA = 0;
		break;
	}
}

ISR(TIMER0_COMPB_vect) {
	// Left right
	// Fake another prescaler divisor of 2
		if (cntB == 0) {
			cntB++;
			return;
		} else {
			cntB = 0;
		}
	switch (counterB++) {
	case 0:
		// Enable pin
		LEFTRIGHTPORT |= (1 << LEFTRIGHTSERVO);
		break;
	case 1:
		// Disable pin
		LEFTRIGHTPORT &= ~(1 << LEFTRIGHTSERVO);
		break;
	case 2:
		OCR0B = 254;
		break;
	case 4:
		counterB = 0;
		return;
	}
}

void rotateInit() {
	// CTC-Mode
	LEFTRIGHTDDR |= (1 << LEFTRIGHTSERVO);
	UPDOWNDDR |= (1 << UPDOWNSERVO); // Enable output drivers
	TCCR0A |= (1 << WGM01); // CTC Mode, OCRA TOP, Update immediate
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64
}

void rotateUpDown(uint8_t pos) {
	UPDOWNREG = pos;
}

void rotateLeftRight(uint8_t pos) {
	LEFTRIGHTREG = pos;
}

void drive(uint16_t cm, uint8_t speed, uint8_t dir) {

#ifdef CMPERTICK
    uint16_t ticks = cm / CMPERTICK;
    if (cm % CMPERTICK != 0) {
        ticks++;
    }
    motorTicks(ticks, ticks);
#endif
#ifdef TICKSPERCM
    motorTicks((cm * TICKSPERCM), (cm * TICKSPERCM));
#endif
    
    motorDirection(dir);
    motorSpeed(speed, speed);
}

void turn(uint16_t degree, uint8_t dir) {
    uint16_t cm = 0;
    while (degree > 360) {
        degree -= 360;
    }
    if (degree != 0) {
        cm = 3 * WHEELDISTANCE * degree; // pi * d * deg/360 = distance to drive
        cm = cm / 360;
    }
    
    drive(cm, TURNSPEED, dir);
}

uint8_t driveDone(void) {
    return motorDone();
}
