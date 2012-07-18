/*
 * motor.h
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

#ifndef motor_h
#define motor_h

// Motor Directions
#define FORWARD 1
#define BACKWARD 2
#define RIGHT 3
#define LEFT 4

// Servo positions
#define CENTER 95
#define LR_CENTER CENTER
#define MIDDLE 50
#define UD_CENTER MIDDLE

#define TURNSPEED 128
// define one, not both
#define CMPERTICK 1
// #define TICKSPERCM 1
// distance between wheels in cm
#define WHEELDISTANCE 20

#define UPDOWNSERVO PB7
#define UPDOWNPORT PORTB
#define UPDOWNDDR DDRB
#define LEFTRIGHTSERVO PG5
#define LEFTRIGHTPORT PORTG
#define LEFTRIGHTDDR DDRG

void driveInit(void);
// dir = FORWARD or BACKWARD
void drive(uint16_t cm, uint8_t speed, uint8_t dir);
// dir = RIGHT or LEFT
void turn(uint16_t degree, uint8_t dir);
uint8_t driveDone(void);

void rotateInit(void);
void rotateUpDown(uint8_t pos); // From 0 to 180
void rotateLeftRight(uint8_t pos);

#endif
