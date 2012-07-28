/*
 * adc.h
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

#ifndef adc_h_
#define adc_h_

#define AVCC 0
#define AREF 1
#define AINT1 2
#define AINT2 3

void adcInit(uint8_t ref);

void adcStart(uint8_t channel);

// Check if conversion is already finished (0 = No; 1 = Yes)
uint8_t adcReady(void);

// Get Conversion Result, start next conversion if next != 0
uint8_t adcGet(uint8_t next);

void adcClose(void);

#endif /* ADC_H_ */
