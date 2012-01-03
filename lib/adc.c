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
#include <adc.h>

void adcInit() {
	// Enable ADC Module, start one conversion, wait for finish
	PRR0 &= ~(1 << PRADC); // Disable ADC Power Reduction (Enable it...)
	ADMUX = (1 << REFS0) | (1 << ADLAR); // Ref: AVCC, left adjust result
	ADCSRA |= (1 << ADEN) | (1 << ADSC); // Enable ADC, start conversion
	while (!adcReady());
    adcGet(0); // Don't start another conversion
}

void adcStart(uint8_t channel) {
	// Start a measurement on channel
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
	// Is the measurement finished
	if ((ADCSRA & (1 << ADSC)) != 0) {
		// ADSC bit is set
		return 0;
	} else {
		return 1;
	}
}

uint8_t adcGet(uint8_t next) {
	// Return measurements result
	// Start next conversion
	uint8_t temp = 0;
	if (adcReady()) {
		temp = ADCH;
		if (next)
			ADCSRA |= (1 << ADSC); // Start next conversion
	}
	return temp;
}

void adcClose() {
	// deactivate adc
	ADCSRA &= ~(1 << ADSC);
    PRR0 |= (1 << PRADC);
    ADCSRA &= ~(1 << ADEN);
}
