/*
 * adc.c
 * Allows you to control the ADC. Interrupt based.
 *
 *  Created on: 06.07.2011
 *      Author: thomas
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
