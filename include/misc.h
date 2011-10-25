/*
 * misc.h
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

#ifndef MISC_H_
#define MISC_H_

#define LCD_ADDRESS 0x66

void ledInit(void);
void ledToggle(uint8_t id);
void ledSet(uint8_t id, uint8_t val);

void lcdPutChar(char c);
void lcdPutString(char* s);
uint8_t lcdGetChar(void);
uint16_t lcdGetNum(void);

// don't free returned string...
char *byteToString(uint8_t byte);

#endif /* MISC_H_ */
