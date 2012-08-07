/*
 * main.c
 *
 * Copyright 2012 Thomas Buck <xythobuz@me.com>
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
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <twi.h>
#include <time.h>
#include <serial.h>
#include <motor.h>
#include <misc.h>
#include <adc.h>
#include <cam.h>
#include <mem.h>
#include <serialHandler.h>
#include <menuHandler.h>
#include <tasks.h>

// Remember: Strings to the lcd should not end with \n
// Timer 0 (8 bit): Servo PWM
// Timer 1 (16bit): Motor Speed PWM
// Timer 2 (8 bit): System Time (Currently unused)
// Timer 3 (16bit): Unused
// Timer 4 (16bit): Unused
// Timer 5 (16bit): Unused

// LED 0 & 1 used as motor display.
// LED 2 should be free...

char buffer[BUFFERSIZE]; // Used as global string buffer
char versionString[] PROGMEM = "xyRobot 1.0\n";
uint8_t upDownPos = MIDDLE;
uint8_t leftRightPos = CENTER;

void resetWatchdog(void) {
	wdt_reset();
}

int main(void) {

	MCUSR = 0;
	wdt_disable();

	ledInit();
	ledToggle(2); // LED on
	driveInit();
	twiInit();
	lcdInit();
	serialInit(UART_BAUD_SELECT(38400,16000000L), 8, NONE, 1);
	adcInit(AINT2); // Voltage reference: 2,56V
	camInit();
	memInit();
	initSystemTimer();

	sei(); // Enable interrupts

	menuInit();
	ledToggle(2); // LED off
	wdt_enable(WDTO_2S); // Watchdog reset after 2s.

	initTasks();
	addTask(&resetWatchdog);
	addTask(&menuHandler);
	addTask(&remoteHandler);
	runTasks();

	return 0;
}
