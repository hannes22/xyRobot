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

volatile uint8_t servoLeftRight = 100;
volatile uint8_t servoUpDown = 100;

// ISR every 10 microseconds
// --> 16.000.000 / Prescaler / OCRValue = 100.000
// --> 16.000.000 /     1     /   160    = 100.000
// --> 16.000.000 /     8     /    20    = 100.000
// Number fix:
// [50;200] = OFFSET + ( [0;180] * FACTOR / QUOTIENT )

// No follow 2 configurations:
// (ISR every 10us or every 100us)
// The commented-out definitions are for
// high-resolution servo movements.
// The now used config needs only 1/10th of
// the CPU-Time but has less resolution.

#define OCRVAL 200 // #define OCRVAL 20
#define COUNTMAX 200 // #define COUNTMAX 2000
#define FACTOR 83
#define QUOTIENT 1000 // #define QUOTIENT 100
#define OFFSET 5 // #define OFFSET 50

void driveInit() {
	motorInit();
	rotateInit();
}

void rotateInit() {
	rotateLeftRight(CENTER);
	rotateUpDown(MIDDLE);

	TCCR0A |= (1 << WGM01); // CTC Mode
	TCCR0B |= (1 << CS01); // Prescaler: 8
	OCR0A = OCRVAL;
	TIMSK0 |= (1 << OCIE0A); // Enable compare match interrupt

	UPDOWNDDR |= (1 << UPDOWNSERVO);
	LEFTRIGHTDDR |= (1 << LEFTRIGHTSERVO);

	UPDOWNPORT &= ~(1 << UPDOWNSERVO);
	LEFTRIGHTPORT &= ~(1 << LEFTRIGHTSERVO);
}

ISR(TIMER0_COMPA_vect) {
	static uint16_t count1, count2;

	if (count1 > servoLeftRight) {
		LEFTRIGHTPORT &= ~(1 << LEFTRIGHTSERVO);
	} else {
		LEFTRIGHTPORT |= (1 << LEFTRIGHTSERVO);
	}

	if (count2 > servoUpDown) {
		UPDOWNPORT &= ~(1 << UPDOWNSERVO);
	} else {
		UPDOWNPORT |= (1 << UPDOWNSERVO);
	}

	if (count1 < COUNTMAX) { // 20ms
		count1++;
		count2++;
	} else {
		count1 = 0;
		count2 = 0;
	}
}

void rotateLeftRight(uint8_t pos) {
	uint16_t tmp = pos * FACTOR;
	tmp /= QUOTIENT;
	servoLeftRight = OFFSET + tmp; // Fix number: (0 - 180) to (50 - 200)
}

void rotateUpDown(uint8_t pos) {
	uint16_t tmp = pos * FACTOR;
	tmp /= QUOTIENT;
	servoUpDown = OFFSET + tmp;
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
