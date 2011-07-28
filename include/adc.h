/*
 * adc.h
 * Allows you to control the ADC. Interrupt based.
 *
 *  Created on: 06.07.2011
 *      Author: thomas
 */

#ifndef ADC_H_
#define ADC_H_

void adcInit(uint8_t channel);

// Check if conversion is already finished (0 = No; 1 = Yes)
uint8_t adcReady(void);

// Get Conversion Result
uint8_t adcGet(void);

void adcClose(void);

#endif /* ADC_H_ */
