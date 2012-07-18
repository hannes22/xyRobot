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
#define MIDDLE 40
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

// ISR every 10 microseconds
// --> 16.000.000 / Prescaler / OCRValue = 100.000
// --> 16.000.000 /     1     /   160    = 100.000
// --> 16.000.000 /     8     /    20    = 100.000
// Number fix:
// [50;200] = OFFSET + ( [0;180] * FACTOR / QUOTIENT )

// No follow 2 configurations:
// (ISR every 10us or every 100us)
// The commented-out definitions are for
// high-resolution servo movements.
// The now used config needs only 1/10th of
// the CPU-Time but has less resolution.

#define OCRVAL 200 // #define OCRVAL 20
#define COUNTMAX 200 // #define COUNTMAX 2000
#define FACTOR 1 // #define FACTOR 83
#define QUOTIENT 12 // #define QUOTIENT 100
#define OFFSET 5 // #define OFFSET 50
#define SERVOSTEPWIDTH QUOTIENT

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
