/*
 * menu.c
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
#include <avr/io.h>

#include "include/misc.h"
#include "include/motor.h"

void menu(void);
void menuDriving(void);
void menuCamera(void);

#define MENUMSGS 3
char messageA[] = "xyRobot Beta\n";
char messageB[] = "1) Driving\n";
char messageC[] = "2) Camera\n";
char *menuMessages[MENUMSGS] = { messageA, messageB, messageC };

void menu(void) {
    uint8_t i;
    uint8_t c;
    uint8_t page = 0;
    
    while (1) {
        lcdPutChar(12); // Clear display
        lcdPutChar(27); // Command Mode
        lcdPutChar(79); // Set cursor position
        lcdPutChar(1); // x (1...20)
        lcdPutChar(1); // y (1...4)
        lcdPutChar(27);
        lcdPutChar(123); // Clear key buffer
        
        for (i = (page * 4); i < MENUMSGS; i++) {
            if (i - (page * 4) > 3) {
                i = MENUMSGS; // cause for to end
                break;
            }
            lcdPutString(menuMessages[i]);
        }
        
        while ((c = lcdGetChar()) == 0);
        switch (page) {
            case '0':
                switch (c) {
                    case '1':
                        menuDriving();
                        break;
                    case '2':
                        menuCamera();
                        break;
            
                    default:
                        lcdPutChar('?');
                        break;
                }
                break;
                
            default:
                break;
        }
    }
}

void menuDriving(void) {
    
}

void menuCamera(void) {
    
}
