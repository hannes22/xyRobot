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
#include <serial.h>
#include <misc.h>

#define FULLTASKMAX 5
#define TIMEDTASKMAX 2

// For real task scheduling...:
void (*tasks[FULLTASKMAX])(void); // Function pointer array
void (*timedTasks[TIMEDTASKMAX])(void);
uint16_t intervalls[TIMEDTASKMAX];
uint8_t shouldExecute[TIMEDTASKMAX];

// For statistics
time_t startTime;
char *taskNames[FULLTASKMAX];
time_t lastStartTimeFullTask[FULLTASKMAX];
time_t absoluteRunTimeFullTask[FULLTASKMAX];

void timer(void);

inline uint8_t countFullTasks(void) {
	uint8_t size = 0;
	while (1) {
		if (size < FULLTASKMAX) {
			if (tasks[size] != NULL) {
				size++;
			} else {
				return size;
			}
		} else {
			return FULLTASKMAX;
		}
	}
	return size;
}

void sendStatistics(void) {
	time_t currentTime = getSystemTime();
	time_t elapsedTime = currentTime - startTime;
	time_t tmp, sum = 0;
	uint8_t fullTasks = countFullTasks();
	uint8_t i, percent = 0;

	serialWriteString("Runtime: ");
	serialWriteString(timeToString(elapsedTime));
	serialWriteString("ms\n");

	for (i = 0; i < fullTasks; i++) {
		serialWriteString("Task ");
		serialWriteString(byteToString(i + 1));
		if (taskNames[i] != NULL) {
			serialWriteString(" (");
			serialWriteString(taskNames[i]);
			serialWrite(')');
		}
		serialWriteString(": ");
		serialWriteString(timeToString(absoluteRunTimeFullTask[i]));
		sum += absoluteRunTimeFullTask[i];
		serialWriteString("ms (");
		tmp = absoluteRunTimeFullTask[i] * 100;
		tmp /= elapsedTime;
		percent += (uint8_t)tmp;
		serialWriteString(timeToString(tmp));
		serialWriteString("%)\n");
	}
	serialWriteString("Scheduling and Interrupts: ");
	serialWriteString(timeToString(elapsedTime - sum));
	serialWriteString("ms (");
	tmp = (elapsedTime - sum) * 100;
	tmp /= elapsedTime;
	serialWriteString(timeToString(tmp));
	serialWriteString("%)\n\n");
}

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
	for (i = 0; i < TIMEDTASKMAX; i++) {
		if (timedTasks[i] != NULL) {
			shouldExecute[i]++;
		} else {
			break;
		}
	}
}

void addFullTimeTask(void (*newTask)(void), char *name) {
	uint8_t i = 0;
	for (i = 0; i < FULLTASKMAX; i++) {
		if (tasks[i] == NULL) {
			tasks[i] = newTask;
			taskNames[i] = name;
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
			lastStartTimeFullTask[currentFullTask] = getSystemTime();
			(*tasks[currentFullTask])();
			absoluteRunTimeFullTask[currentFullTask] += (getSystemTime() - lastStartTimeFullTask[currentFullTask]);
		}

		// Determine next full task
		if (currentFullTask < (FULLTASKMAX - 1)) {
			currentFullTask++;
			if (tasks[currentFullTask] == NULL) {
				currentFullTask = 0;
			}
		} else {
			currentFullTask = 0;
		}
	}
}
