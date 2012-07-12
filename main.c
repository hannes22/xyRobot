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

#include <twi.h>
#include <serial.h>
#include <motor.h>
#include <misc.h>
#include <adc.h>
#include <cam.h>
#include <remoteHandler.h>

//void menu(void);

// Remember: Strings to the lcd should not end with \n

int main(void) {

	// twiInit();
	// lcdInit();
	ledInit();
	serialInit(UART_BAUD_SELECT(38400,16000000L), 8, NONE, 1);
	driveInit();
	adcInit();
    
	sei();

	ledFlash();
	serialWriteString("Initialized!\n");

//	menu();
    
	while(1) {
        remoteHandler();
        ledToggle(2);
	}

	return 0;
}
