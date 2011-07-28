/*
 * motor.c
 *
 *  Created on: 17.07.2011
 *      Author: thomas
 */

#include <avr/io.h>
#include <stdint.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

#include "include/motor.h"

volatile uint8_t dirGlobal = FORWARD;
volatile uint8_t speedRight = 0;
volatile uint8_t speedLeft = 0;
volatile int32_t countRight = 0;
volatile int32_t countLeft = 0;

#define LEFTSP OCR1B
#define RIGHTSP OCR1A

void motorInit() {
	DDRB |= (1 << DDB5) | (1 << DDB6); // PWM Ports als Ausgang
	DDRL |= (1 << DDL7) | (1 << DDL6);
	DDRJ |= (1 << DDJ7) | (1 << DDJ6);

	TCCR1A |= (1 << COM1A1) | (1 << COM1B1);
	TCCR1A |= (1 << WGM10);
	TCCR1B |= (1 << WGM12) | (1 << CS11);

	EICRB |= (1 << ISC61) | (1 << ISC60) | (1 << ISC71) | (1 << ISC70); // Rising Edge...
	EIMSK |= (1 << INT7) | (1 << INT6); // ...activates Interrupts

}

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
			if (dirGlobal == FORWARD) {
				diff = (countRight + countLeft);
			} else {
				diff = ((countRight * (-1)) + (countLeft * (-1)));
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
	calcDiff();
}

ISR(INT7_vect) { // Left Encoder
	if ((dirGlobal == FORWARD) || (dirGlobal == TURNRIGHT)) {
		countLeft++;
	} else if ((dirGlobal == BACKWARD) || (dirGlobal == TURNLEFT)) {
		countLeft--;
	}
	//calcDiff();
}

void motorSpeed(uint8_t left, uint8_t right) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		speedLeft = left;
		speedRight = right;
		OCR1B = (uint16_t)speedLeft;
		OCR1A = (uint16_t)speedRight;
		countRight = 0;
		countLeft = 0;
	}
}

void motorStop() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		OCR1A = 0;
		OCR1B = 0;
		speedLeft = 0;
		speedRight = 0;
		countRight = 0;
		countLeft = 0;
	}
}

void motorDirection(uint8_t dir) {
	switch (dir) {
	case FORWARD: case TURNRIGHT:
		PORTL &= ~((1 << PL6) | (1 << PL7));
		PORTL |= (1 << PL6);
		break;
	case BACKWARD: case TURNLEFT:
		PORTL &= ~((1 << PL6) | (1 << PL7));
		PORTL |= (1 << PL7);
		break;
	}
	switch (dir) {
	case FORWARD: case TURNLEFT:
		PORTJ &= ~((1 << PJ6) | (1 << PJ7));
		PORTJ |= (1 << PJ7);
		break;
	case BACKWARD: case TURNRIGHT:
		PORTJ &= ~((1 << PJ6) | (1 << PJ7));
		PORTJ |= (1 << PJ6);
		break;
	}
	dirGlobal = dir;
}
