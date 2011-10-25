/*
 * adc.c
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
#include "include/adc.h"

void adcInit() {
	PRR0 &= ~(1 << PRADC); // Disable ADC Power Reduction (Enable it...)
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR); // Ref: AVCC, left adjust result
	ADCSRA |= (1 << ADEN) | (1 << ADSC); // Enable ADC
	while (!adcReady());
    adcGet();
}

void adcStart(uint8_t channel) {
    if (channel > 15) {
		channel = 0;
	}
	if (channel > 7) {
		channel -= 8;
		ADCSRB |= (1 << MUX5);
	}
	ADMUX |= channel;
    ADCSRA |= (1 << ADSC);
}

uint8_t adcReady() {
	if ((ADCSRA & (1 << ADSC)) != 0) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t adcGet() {
	if (adcReady()) {
		ADCSRA |= (1 << ADSC); // Clear flag
		return ADCH;
	} else {
		return 0;
	}
}

void adcClose() {
	ADCSRA &= ~(1 << ADSC);
    PRR0 |= (1 << PRADC);
}
