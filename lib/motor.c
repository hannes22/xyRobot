/*
 * motor.c
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

#include <stdint.h>

#include "motor.h"
#include "motor_low.h"

#ifdef CMPERTICK
#ifdef TICKSPERCM
#error "DEFINE ONLY ONE CONSTANT!"
#endif
#endif

void driveInit() {
    motorInit();
}

void drive(uint16_t cm, uint8_t speed, uint8_t dir) {
#ifdef CMPERTICK
    uint16_t ticks = cm / CMPERTICK;
    if (cm % CMPERTICK != 0) {
        ticks++;
    }
    motorTicks(ticks, ticks);
#else
    motorTicks((cm * TICKSPERCM), (cm * TICKSPERCM));
#endif
    
    motorDirection(dir);
    motorSpeed(speed, speed);
}

void turn(uint16_t degree, uint8_t dir) {
    uint16_t cm = 0;
    while (degree > 360) {
        degree -= 360;
    }
    if (degree != 0) {
        cm = 3 * WHEELDISTANCE * degree; // pi * d * (deg/360) = distance to drive
        cm = cm / 360;
        if ((cm % 360) != 0) {
            cm++; // If rounding error, we turn more than needed
        }
    }
    
    drive(cm, TURNSPEED, dir);
}

uint8_t driveDone(void) {
    return motorDone();
}
