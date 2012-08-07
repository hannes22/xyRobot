/*
 * mem.c
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
#include <mem.h>

#include <avr/wdt.h>

void memInit() {
	DDRC = 0xFF; // PC Output
	DDRJ |= 63; // PJ0...PJ5 Output
	DDRK |= 48; // PK4 + PK5 Output

	PORTC = 0;
	PORTJ &= ~(63);
	PORTK &= ~(1 << PK5);
	PORTK |= (1 << PK4); // Read mode
}

void setAddress(uint32_t a) {
	PORTJ |= (1 << PJ0); // Latch 1
	PORTC = (uint8_t)(a & 0xFF); // A0 to A7
	asm volatile ("nop");
	PORTJ &= ~(1 << PJ0);
	PORTJ |= (1 << PJ1); // Latch 2
	PORTC = (uint8_t)((a & 0xFF00) >> 8); // A8 to A15
	asm volatile ("nop");
	PORTJ &= ~(1 << PJ1);

	if (a & 0x10000) { // A16
		PORTJ |= (1 << PJ2);
	} else {
		PORTJ &= ~(1 << PJ2);
	}

	if (a & 0x20000) { // A17
		PORTJ |= (1 << PJ3);
	} else {
		PORTJ &= ~(1 << PJ3);
	}

	if (a & 0x40000) { // A18
		PORTJ |= (1 << PJ4);
	} else {
		PORTJ &= ~(1 << PJ4);
	}

	PORTK ^= (1 << PK5); // Toggle led
}

uint8_t memGet(uint32_t a) {
	uint8_t val;

	if (a >= MEMSIZE) {
		a = MEMSIZE - 1;
	}
	setAddress(a);
	DDRC = 0; // Input
	asm volatile ("nop");
	val = PINC;
	DDRC = 0xFF; // Output again
	return val;
}

void memSet(uint32_t a, uint8_t d) {
	if (a >= MEMSIZE) {
		a = MEMSIZE - 1;
	}
	setAddress(a);
	PORTK &= ~(1 << PK4); // Write mode
	PORTC = d;
	PORTK |= (1 << PK4); // Back to read mode
}

uint8_t memCalcErrorRate(void) {
	uint32_t i, val;

	for (i = 0; i < MEMSIZE; i++) {
		wdt_reset();
		val = i % 256;
		memSet(i, val);
	}

	return memCheckErrorRateAgain();
}

uint8_t memCheckErrorRateAgain(void) {
	uint32_t i, val, errors = 0, calc;

	for (i = 0; i < MEMSIZE; i++) {
		wdt_reset();
		val = i % 256;
		if (memGet(i) != val) {
			errors++;
		}
	}

	calc = 100 * errors;
	calc = calc / MEMSIZE;
	return (uint8_t)calc;
}
