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

#include <util/delay.h>

void driveInit() {
    motorInit();
    rotateInit();
    rotateLeftRight(LR_CENTER);
    rotateUpDown(UD_CENTER);
}

void rotateInit() {
	// 8-bit Counter
	TCCR0A |= (1 << WGM00) | (1 << WGM01); // Fast PWM. Top = 0xFF
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // Non inverting mode
	TCCR0B |= (1 << CS02) | (1 << CS00); // Prescaler 1024 => roughly 20ms per round

	UPDOWNDDR |= (1 << UPDOWNSERVO);
	LEFTRIGHTDDR |= (1 << LEFTRIGHTSERVO);
}

void rotateUpDown(uint8_t pos) {
	uint16_t mid = pos * 15;
	UPDOWNREG = ((uint8_t)(mid / 256)) + 16;
}

void rotateLeftRight(uint8_t pos) {
	uint16_t mid = pos * 15;
	LEFTRIGHTREG = ((uint8_t)(mid / 256)) + 16;
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
