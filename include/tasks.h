/*
 * tasks.h
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

#ifndef tasks_h_
#define tasks_h_

void initTasks(void);
void addFullTimeTask(void (*newTask)(void), char *name);

// Executed every intervall*128ms
// intervall=4 --> Executed every 512ms
void addTimedTask(void (*newTask)(void), uint16_t intervall);

void runTasks(void);

void sendStatistics(void);

#endif
