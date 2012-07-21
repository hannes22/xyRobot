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

#ifndef misc_h_
#define misc_h_

#define LCD_ADDRESS 0x42

void ledInit(void);
void ledToggle(uint8_t id);
void ledSet(uint8_t id, uint8_t val);
void ledFlash(void);

void lcdInit(void);
void lcdSetBackgroundLight(uint8_t status);
void lcdPutChar(char c);
void lcdPutString(char* s);
uint8_t lcdGetChar(void);
uint16_t lcdGetNum(void);

// Uses buffer[] as return value.
char *byteToString(uint8_t byte);
char *bytesToString(uint16_t bytes);
char *byteToHex(uint8_t byte);

#endif /* MISC_H_ */
