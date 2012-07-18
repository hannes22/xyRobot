/*
 * motor_low.c
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
#include <avr/io.h>
#include <stdint.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

#include <motor_low.h>
#include <misc.h>

volatile uint8_t dirGlobal = FORWARD; // Motor Direction
volatile uint8_t speedRight = 0;
volatile uint8_t speedLeft = 0;
volatile int32_t countRight = 0; // Tick counters
volatile int32_t countLeft = 0;
volatile uint16_t remainRight = 0; // Ticks remaining
volatile uint16_t remainLeft = 0;

#define LEFTSP OCR1B
#define RIGHTSP OCR1A

void motorTicks(uint16_t left, uint16_t right) {
    remainRight = right;
    remainLeft = left;
}

uint8_t motorDone() {
    if ((remainRight == 0) && (remainLeft == 0)) {
        return 1;
    } else {
        return 0;
    }
}

void motorInit() {
	DDRB |= (1 << DDB5) | (1 << DDB6); // PWM Ports as outputs
	DDRL |= (1 << DDL7) | (1 << DDL6);
	DDRJ |= (1 << DDJ7) | (1 << DDJ6);

	TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
	TCCR1A |= (1 << WGM10);
	TCCR1B |= (1 << WGM12) | (1 << CS11);

	EICRB |= (1 << ISC61) | (1 << ISC60) | (1 << ISC71) | (1 << ISC70); // Rising Edge...
	EIMSK |= (1 << INT7) | (1 << INT6); // ...activates Interrupts
}

// Adjust motor speed directly in the timer registers.
// Doesn't change global vars speedLeft/Right
void calcDiff(void) {
	int32_t diff;
	uint8_t speed;
	if ((speedLeft == speedRight) && (speedLeft != 0) && (countRight != countLeft) && (countRight > 1)) {
		speed = speedLeft;
		if ((dirGlobal == FORWARD) || (dirGlobal == BACKWARD)) {
			if (dirGlobal == FORWARD) {
				diff = (countRight - countLeft);
			} else {
				diff = ((countRight * (-1)) - (countLeft * (-1)));
			}
			if (diff > 0) {
				if ((LEFTSP < (speed + 25)) && (LEFTSP < 255)) {
					LEFTSP++;
				} else if ((RIGHTSP > 1) && (RIGHTSP > (speed - 25))) {
					RIGHTSP--;
				}
			} else if (diff < 0) {
				if ((RIGHTSP < (speed + 25)) && (RIGHTSP < 255)) {
					RIGHTSP++;
				} else if ((LEFTSP > 1) && (LEFTSP > (speed - 25))) {
					LEFTSP--;
				}
			}
		} else if ((dirGlobal == TURNLEFT) || (dirGlobal == TURNRIGHT)) {
            if (dirGlobal == TURNLEFT) {
				diff = (countRight - (countLeft * (-1)));
			} else {
				diff = ((countRight * (-1)) - countLeft);
			}
			if (diff > 0) {
				if ((LEFTSP < (speed + 25)) && (LEFTSP < 255)) {
					LEFTSP++;
				} else if ((RIGHTSP > 1) && (RIGHTSP > (speed - 25))) {
					RIGHTSP--;
				}
			} else if (diff < 0) {
				if ((RIGHTSP < (speed + 25)) && (RIGHTSP < 255)) {
					RIGHTSP++;
				} else if ((LEFTSP > 1) && (LEFTSP > (speed - 25))) {
					LEFTSP--;
				}
			}
		}
		countLeft = 0;
		countRight = 0;
	}
}

ISR(INT6_vect) { // Right Encoder
	if ((dirGlobal == FORWARD) || (dirGlobal == TURNLEFT)) {
		countRight++;
	} else if ((dirGlobal == BACKWARD) || (dirGlobal == TURNRIGHT)) {
		countRight--;
	}
    if (remainRight > 0) {
        remainRight--;
        if (remainRight == 0) {
            speedRight = 0;
            motorSpeed(speedLeft, 0);
            ledSet(1, 0);
        }
    }
	calcDiff(); // Adjust for slightly different motor speed
}

ISR(INT7_vect) { // Left Encoder
	if ((dirGlobal == FORWARD) || (dirGlobal == TURNRIGHT)) {
		countLeft++;
	} else if ((dirGlobal == BACKWARD) || (dirGlobal == TURNLEFT)) {
		countLeft--;
	}
    if (remainLeft > 0) {
        remainLeft--;
        if (remainLeft == 0) {
            speedLeft = 0;
            motorSpeed(0, speedRight);
            ledSet(0, 0);
        }
    }
}

void motorSpeed(uint8_t left, uint8_t right) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		speedLeft = left;
		speedRight = right;
		LEFTSP = (uint16_t)speedLeft;
		RIGHTSP = (uint16_t)speedRight;
		countRight = 0;
		countLeft = 0;
	}
	if (left != 0) {
		ledSet(0, 1);
	} else {
		ledSet(0, 0);
		motorDirection(BREAKLEFT);
	}
	if (right != 0) {
		ledSet(1, 1);
	} else {
		ledSet(1, 0);
		motorDirection(BREAKRIGHT);
	}
}

void motorStop() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		LEFTSP = 0;
		RIGHTSP = 0;
		speedLeft = 0;
		speedRight = 0;
		countRight = 0;
		countLeft = 0;
	}
	motorDirection(BREAK);
	ledSet(0, 0);
	ledSet(1, 0);
}

void motorDirection(uint8_t dir) {
	// Left motor
	switch (dir) {
	case FORWARD: case TURNRIGHT:
		PORTL &= ~((1 << PL6) | (1 << PL7));
		PORTL |= (1 << PL6);
		break;
	case BACKWARD: case TURNLEFT:
		PORTL &= ~((1 << PL6) | (1 << PL7));
		PORTL |= (1 << PL7);
		break;
	case BREAK: case BREAKLEFT:
		PORTL &= ~((1 << PL6) | (1 << PL7));
		break;
	}
	// Right motor
	switch (dir) {
	case FORWARD: case TURNLEFT:
		PORTJ &= ~((1 << PJ6) | (1 << PJ7));
		PORTJ |= (1 << PJ7);
		break;
	case BACKWARD: case TURNRIGHT:
		PORTJ &= ~((1 << PJ6) | (1 << PJ7));
		PORTJ |= (1 << PJ6);
		break;
	case BREAK: case BREAKRIGHT:
		PORTJ &= ~((1 << PJ6) | (1 << PJ7));
		break;
	}
	dirGlobal = dir;
}
