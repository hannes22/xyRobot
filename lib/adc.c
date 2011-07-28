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

void adcInit(uint8_t channel) {
	PRR0 &= ~(1 << PRADC); // Disable ADC Power Reduction (Enable it...)
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR); // Ref: AVCC, left adjust result
	if (channel > 15) {
		channel = 0;
	}
	if (channel > 7) {
		channel -= 8;
		ADCSRB |= (1 << MUX5);
	}
	ADMUX |= channel;
	ADCSRA |= (1 << ADEN) | (1 << ADSC); // Enable ADC
	while (!adcReady());
    adcGet();
}

uint8_t adcReady() {
	if ((ADCSRA & (1 << ADIF)) != 0) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t adcGet() {
	if (adcReady()) {
		ADCSRA |= (1 << ADIF); // Clear flag
		return ADCH;
	} else {
		return 0;
	}
}

void adcClose() {
	ADCSRA &= ~(1 << ADSC);
}
