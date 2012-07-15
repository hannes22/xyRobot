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

#define WRITE PORTK|=(1<<PK4)
#define READ PORTK&=~(1<<PK4)

#define MEMSIZE ((uint32_t)512 * (uint32_t)1024)

void memInit() {
	DDRC = 0xFF; // PC Output
	DDRJ |= 63; // PJ0...PJ5 Output
	DDRK |= 48; // PK4 + PK5 Output

	PORTJ &= ~(1 << PJ5); // Select first (and only) SRAM
	// Bank-Switching not yet implemented
}

void selectLatch(uint8_t l) {
	PORTJ &= ~((1 << PJ0) | (1 << PJ1)); // Clear latch selection
	if (l == 0) {
		PORTJ |= (1 << PJ0);
	} else if (l == 1) {
		PORTJ |= (1 << PJ1);
	}
}

void setLatch(uint8_t l, uint8_t d) {
	selectLatch(l);
	PORTC = d;
}

void setAddress(uint32_t a) {
	setLatch(0, (uint8_t)(a & 0xFF)); // A0 to A7
	setLatch(1, (uint8_t)((a & 0xFF00) >> 8)); // A8 to A15
	selectLatch(3); // No latch activated
	if (a & ((uint32_t)1 << 16)) { // A16
		PORTJ |= (1 << PJ2);
	} else {
		PORTJ &= ~(1 << PJ2);
	}
	if (a & ((uint32_t)1 << 17)) { // A17
		PORTJ |= (1 << PJ3);
	} else {
		PORTJ &= ~(1 << PJ3);
	}
	if (a & ((uint32_t)1 << 18)) { // A18
		PORTJ |= (1 << PJ4);
	} else {
		PORTJ &= ~(1 << PJ4);
	}
}

uint8_t memGet(uint32_t a) {
	uint8_t val;

	if (a >= MEMSIZE) {
		a = MEMSIZE - 1;
	}
	setAddress(a);
	PORTK |= (1 << PK4); // Set read mode
	DDRC = 0; // Input
	val = PINC;
	DDRC = 0xFF; // Output again
	PORTK ^= (1 << PK5); // Toggle led
	return val;
}

void memSet(uint32_t a, uint8_t d) {
	if (a >= MEMSIZE) {
		a = MEMSIZE - 1;
	}
	setAddress(a);
	PORTK &= ~(1 << PK4); // Write mode
	PORTC = d;
	PORTK ^= (1 << PK5); // Toggle led
}
