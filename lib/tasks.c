/*
 * tasks.c
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
#include <stdlib.h>
#include <avr/wdt.h>

#include <tasks.h>
#include <time.h>

#define FULLTIMETASKMAX 2
#define TIMEDTASKMAX 1

void (*tasks[FULLTIMETASKMAX])(void); // Function pointer array
void (*timedTasks[TIMEDTASKMAX])(void);
uint16_t intervalls[TIMEDTASKMAX];
uint8_t shouldExecute[TIMEDTASKMAX];

void timer(void) {
	uint8_t i;
	for (i = 0; i < TIMEDTASKMAX; i++) {
		if (timedTasks[i] != NULL) {
			shouldExecute[i]++;
		} else {
			break;
		}
	}
}

void initTasks(void) {
	uint8_t i;
	for (i = 0; i < FULLTIMETASKMAX; i++) {
		tasks[i] = NULL;
	}
	for (i = 0; i < TIMEDTASKMAX; i++) {
		timedTasks[i] = NULL;
		intervalls[i] = 0;
		shouldExecute[i] = 0;
	}
	setTimedCall(&timer, 7); // timer is called every 128ms
}

void addFullTimeTask(void (*newTask)(void)) {
	uint8_t i = 0;
	for (i = 0; i < FULLTIMETASKMAX; i++) {
		if (tasks[i] == NULL) {
			tasks[i] = newTask;
			break;
		}
	}
}

void addTimedTask(void (*newTask)(void), uint16_t intervall) {
	uint8_t i = 0;
	for (i = 0; i < TIMEDTASKMAX; i++) {
		if (timedTasks[i] == NULL) {
			timedTasks[i] = newTask;
			intervalls[i] = intervall;
			break;
		}
	}
}

void runTasks(void) {
	uint8_t currentFullTask = 0;
	uint8_t i;
	while (1) {
		wdt_reset();

		// Were some timed tasks marked for execution?
		for (i = 0; i < TIMEDTASKMAX; i++) {
			if (timedTasks[i] != NULL) {
				if (shouldExecute[i] >= intervalls[i]) {
					(*timedTasks[i])();
					shouldExecute[i] = 0;
				}
			} else {
				break; // No more tasks following...
			}
		}

		// Execute next full-time task
		if (tasks[currentFullTask] != NULL) {
			(*tasks[currentFullTask])();
		}

		// Determine next full task
		if (currentFullTask < (FULLTIMETASKMAX - 1)) {
			currentFullTask++;
			if (tasks[currentFullTask] == NULL) {
				currentFullTask = 0;
			}
		} else {
			currentFullTask = 0;
		}
	}
}
