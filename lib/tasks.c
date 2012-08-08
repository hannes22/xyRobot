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

#define FULLTASKMAX 5
#define TIMEDTASKMAX 2

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

void sort(uint8_t **percent, uint8_t size) {
	uint8_t i, j, *t;

	if (size <= 1) {
		return;
	}

	for (i = 0; i < size - 1; i++) {
		for (j = 0; j < size - i - 1; j++) {
			if (percent[j][0] > percent[j + 1][0]) {
				t = percent[j];
				percent[j] = percent[j + 1];
				percent[j + 1] = t;
			}
		}
	}
}

void sendStatistics(void) {
	time_t currentTime = getSystemTime();
	time_t elapsedTime = currentTime - startTime;
	time_t tmp;
	uint8_t i, sum = 0, **percent;

	percent = (uint8_t **)malloc((fullTasksRegistered + 1) * sizeof(uint8_t *));
	if (percent == NULL) {
		serialWriteString("Sorry, not enough memory!\n");
		return;
	}

	for (i = 0; i < fullTasksRegistered; i++) {
		percent[i] = (uint8_t *)malloc(2 * sizeof(uint8_t));
		if (percent[i] == NULL) {
			serialWriteString("Sorry, not enough memory!\n");
			for (sum = 0; sum < i; sum++) {
				free(percent[sum]);
			}
			free(percent);
			return;
		}
		tmp = absoluteRunTimeFullTask[i] * 100;
		tmp /= elapsedTime;
		percent[i][0] = (uint8_t)tmp;
		sum += percent[i][0];
		percent[i][1] = i;
	}
	percent[fullTasksRegistered] = (uint8_t *)malloc(2 * sizeof(uint8_t));
	if (percent[fullTasksRegistered] == NULL) {
		serialWriteString("Sorry, not enough memory!\n");
		for (sum = 0; sum < fullTasksRegistered; sum++) {
			free(percent[sum]);
		}
		free(percent);
		return;
	}
	percent[fullTasksRegistered][1] = FULLTASKMAX;
	percent[fullTasksRegistered][0] = 100 - sum;

	sort(percent, FULLTASKMAX + 1);
	
	serialWriteString("CPU  -  Time  -  Task\n");
	for (i = 0; i < (fullTasksRegistered + 1); i++) {
		serialWriteString(byteToString(percent[i][0]));
		serialWriteString("%  -  ");
		serialWriteString(timeToString(absoluteRunTimeFullTask[percent[i][1]]));
		serialWriteString("ms  -  (");
		if (i < fullTasksRegistered) {
			serialWriteString(byteToString(percent[i][1] + 1));
			serialWriteString(") ");
			if (taskNames[i] != NULL) {
				serialWriteString(taskNames[i]);
			} else {
				serialWriteString("??");
			}
		} else {
			serialWriteString("0) Idle");
		}
		serialWriteString("\n");
	}

	for (i = 0; i < (fullTasksRegistered + 1); i++) {
		free(percent[i]);
	}
	free(percent);
}
