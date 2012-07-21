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
 #include <adc.h>

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

void driveInit() {
	motorInit();
	rotateInit();
}

// Calc Distances:
// |------------------------------------|
// | dist |Voltage| ADC  |	Formula		|
// |------|-------|------|--------------|
// |10cm  | 2,6V  | 133  | <--|			|
// |	  |  	  | 	 |    |--> d1(x)|
// |45cm  | 0,7V  | 36   | <--|			|
// |	  |  	  | 	 |    |--> d2(x)|
// |80cm  | 0,4V  | 20   | <--|			|
// |------------------------------------|
// 		--> d1(x) = 58 - 0,36x
// 		--> d2(x) = 124 - 2,19x
uint8_t getDistance() {
	uint16_t val, ret;
	uint8_t val2;
	adcStart(10); // Sharp Sensor on ADC10
	val = adcGet(1); // Start another conversion
	val2 = adcGet(0); // Finished
	val = (val + val2) / 2;
	if (val >= 133) {
		ret = 80; // 80cm, max distance
	} else if (val >= 36) {
		ret = 58 - ((36 * val) / 100);
	} else if (val >= 20) {
		ret = 124 - ((219 * val) / 100);
	} else {
		ret = 0;
	}
	return (uint8_t)ret;
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
	uint16_t tmp = (180 - pos) * FACTOR;
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
