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

#include <tasks.h>

#define tasksLength 3
void (*tasks[tasksLength])(void); // Function pointer array

void initTasks(void) {
	uint8_t i;
	for (i = 0; i < tasksLength; i++) {
		tasks[i] = NULL;
	}
}

void addTask(void (*newTask)(void)) {
	uint8_t i = 0;
	for (i = 0; i < tasksLength; i++) {
		if (tasks[i] == NULL) {
			tasks[i] = newTask;
			break;
		}
	}
}

void runTasks(void) {
	uint8_t currentTask = 0;
	while (1) {
		if (tasks[currentTask] != NULL) {
			(*tasks[currentTask])();
		}
		if (currentTask < (tasksLength - 1)) {
			currentTask++;
		} else {
			currentTask = 0;
		}
	}
}
