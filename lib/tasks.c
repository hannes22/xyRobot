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
#include <string.h>

#include <tasks.h>
#include <time.h>
#include <serial.h>
#include <misc.h>

#define FULLTASKMAX 3
#define TIMEDTASKMAX 3

/*
 * So what's up with this Task-Managing stuff anyways?
 * In this file, the main infinite loop is implemented.
 * Different registered tasks are executed one after another.
 * In between, we can execute scheduled tasks. They can be scheduled
 * on a time interval of multiples of 128ms.
 * But why this strange number?
 * To do this, we have code in every overflow of our system timer.
 * This happens every millisecond.
 * This code has to do a really expensive modulo operation to determine if
 * it is time to execute the task scheduling code. If we only allow time intervalls of
 * powers of two, we can reduce this modulo to an easy a & (2^x - 1)
 * which, thanks to binary, is equal to a & ((1 << x) - 1)
 * Now we don't have to time our events on a time scale smaller than 100ms.
 * So I have chosen 128ms as next best power of two.
 */

// For real task scheduling...:
void (*tasks[FULLTASKMAX])(void); // Function pointer array
uint8_t fullTasksRegistered = 0;
void (*timedTasks[TIMEDTASKMAX])(void);
uint16_t intervalls[TIMEDTASKMAX];
uint8_t shouldExecute[TIMEDTASKMAX];
uint8_t timedTasksRegistered = 0;

// For statistics
time_t startTime;
char *taskNames[FULLTASKMAX];
time_t lastStartTimeFullTask[FULLTASKMAX];
time_t absoluteRunTimeFullTask[FULLTASKMAX];

void timer(void);

void initTasks(void) {
	uint8_t i;
	for (i = 0; i < FULLTASKMAX; i++) {
		tasks[i] = NULL;
		lastStartTimeFullTask[i] = 0;
		absoluteRunTimeFullTask[i] = 0;
		taskNames[i] = 0;
	}
	for (i = 0; i < TIMEDTASKMAX; i++) {
		timedTasks[i] = NULL;
		intervalls[i] = 0;
		shouldExecute[i] = 0;
	}
	startTime = getSystemTime();
	setTimedCall(&timer, 7); // timer is called every 128ms
}

void timer(void) {
	uint8_t i;
	for (i = 0; i < timedTasksRegistered; i++) {
		if (timedTasks[i] != NULL) {
			shouldExecute[i]++;
		}
	}
}

void addFullTimeTask(void (*newTask)(void), char *name) {
	if (fullTasksRegistered < FULLTASKMAX) {
		tasks[fullTasksRegistered] = newTask;
		taskNames[fullTasksRegistered] = name;
		fullTasksRegistered++;
	}
}

void addTimedTask(void (*newTask)(void), uint16_t intervall) {
	if (timedTasksRegistered < TIMEDTASKMAX) {
		timedTasks[timedTasksRegistered] = newTask;
		intervalls[timedTasksRegistered] = intervall;
		timedTasksRegistered++;
	}
}

void runTasks(void) {
	uint8_t currentFullTask = 0;
	uint8_t i;
	while (1) {
		// Were some timed tasks marked for execution?
		for (i = 0; i < timedTasksRegistered; i++) {
			if (timedTasks[i] != NULL) {
				if (shouldExecute[i] >= intervalls[i]) {
					(*timedTasks[i])();
					shouldExecute[i] = 0;
				}
			}
		}

		// Execute next full-time task
		if (tasks[currentFullTask] != NULL) {
			lastStartTimeFullTask[currentFullTask] = getSystemTime();
			(*tasks[currentFullTask])();
			absoluteRunTimeFullTask[currentFullTask] += (getSystemTime() - lastStartTimeFullTask[currentFullTask]);
		}

		// Determine next full task
		if (currentFullTask < (fullTasksRegistered - 1)) {
			currentFullTask++;
		} else {
			currentFullTask = 0;
		}
	}
}

// ----------------- Statistics -----------------

/*
 * Returned data structure: data[fullTasksRegistered]
 * data[x][0]      : Process id
 * data[x][1]      : CPU Time in percent
 *
 * data == NULL on error, eg. not enough memory
 *
 * free(data) after usage!
 */
uint8_t **getStatistics(void) {
	uint8_t **data;
	uint8_t i, j;
	time_t percent, time = getSystemTime();

	data = (uint8_t **)malloc(fullTasksRegistered * sizeof(uint8_t *));
	if (data == NULL) {
		return NULL;
	}
	for (i = 0; i < fullTasksRegistered; i++) {
		data[i] = (uint8_t *)malloc(2 * sizeof(uint8_t));
		if (data[i] == NULL) {
			for (j = 0; j < i; j++) {
				free(data[j]);
			}
			free(data);
			return NULL;
		}

		percent = absoluteRunTimeFullTask[i] * 100;
		percent = percent / diffTime(time, startTime);

		data[i][0] = i;
		data[i][1] = percent;
	}

	return data;
}

uint8_t sum(uint8_t **data, uint8_t index, uint8_t length) {
	uint8_t i, s = 0;
	for (i = 0; i < length; i++) {
		s += data[i][index];
	}
	return s;
}

void sendStatistics(void) {
	uint8_t i;
	uint8_t **data = getStatistics();

	serialWriteString("PID - CPU - Name\n");
	for (i = 0; i < fullTasksRegistered; i++) {
		serialWrite(' ');
		serialWriteString(byteToString(data[i][0]));
		serialWriteString("  - ");
		serialWriteString(byteToString(data[i][1]));
		serialWriteString("% ");
		if (data[i][1] < 10) {
			serialWrite(' ');
		}
		serialWriteString("- ");
		serialWriteString(taskNames[data[i][0]]);
		serialWrite('\n');
	}
	serialWriteString(" X  - ");
	serialWriteString(byteToString(100 - sum(data, 1, fullTasksRegistered)));
	serialWriteString("% - Timed Tasks\n\n");

	for (i = 0; i < fullTasksRegistered; i++) {
		free(data[i]);
	}
	free(data);
}
