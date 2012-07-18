/*
 * motor_low.h
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

#ifndef motor_low_h_
#define motor_low_h_

#define FORWARD 1
#define BACKWARD 2
#define TURNRIGHT 3
#define TURNLEFT 4
#define BREAK 5
#define BREAKRIGHT 6
#define BREAKLEFT 7

void motorInit(void);
void motorSpeed(uint8_t left, uint8_t right);
void motorStop(void);
void motorDirection(uint8_t dir);
void motorTicks(uint16_t left, uint16_t right);
uint8_t motorDone(void);

#endif /* MOTOR_LOW_H_ */
