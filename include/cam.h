/*
 * cam.h
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

#ifndef cam_h
#define cam_h

#define CAMPORT PORTA
#define CAMPIN PINA
#define CAMDDR DDRA
#define CAMSTART PA5
#define CAMDATA PA4
#define CAMLOAD PA3
#define CAMRESET PA2
#define CAMCLOCK PA1
#define CAMREAD PA0
#define CAMOUT 11

void camInit(void);
void camShoot(uint8_t *regs);
uint8_t camGetByte(void);

void camStore(uint8_t *regs, uint8_t pos); // Store in memory at (pos * 128 * 128)
void camSendStored(uint8_t pos, uint8_t depth); // Depth in bit/pixel. Possible: 8, 4, 2, 1

#endif
