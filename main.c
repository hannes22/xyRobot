/*
 * main.c
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

#define puts(x) lcdPutString(x)

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "include/twi.h"
#include "include/serial.h"
#include "include/motor.h"
#include "include/misc.h"
#include "include/adc.h"
#include "include/cam.h"

void menu(void);

int main(void) {

	twiInit();
	ledInit();
	serialInit(51, 8, NONE, 1);
	driveInit();
    adcInit();
    
	sei();
    
    menu();
    
	while(1) {
        lcdPutString("\nRESET ME!");
	}

	return 0;
}
