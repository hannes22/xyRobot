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
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include <misc.h>
#include <motor.h>

/*
 * This menu certainly handles more than 10 Menu Entries (including the header),
 * but it is not very pretty... Be advised!
 */

void menu(void);
void menuDriving(void);
void menuCamera(void);
void menuBluetooth(void);

#define MENUMSGS 4
// 3 entries per page
// Number 1 starts with second entry
// If you reach 9, continue with 1 again, insert empty line
// Start with \n
// page 1
char messageA[] = "\nxyRobot Beta";
char messageB[] = "\n1) Driving";
char messageC[] = "\n2) Camera";
// page 2
char messageD[] = "\n3) Bluetooth";

char *menuMessages[MENUMSGS] = { messageA, messageB, messageC, messageD };

// Function pointers for messages. entry 0, 10, 20... NULL because it is not selectable
void (*menuFunctions[MENUMSGS])(void) = { NULL, &menuDriving, &menuCamera, &menuBluetooth };

void menu(void) {
	uint8_t temp;
	uint8_t i = 0;
    uint16_t c;
    uint8_t page = 0; // a page has 3 entries

    while (1) {
        // print entries
    	if ((page * 3) < MENUMSGS)
    		lcdPutString(menuMessages[page * 3]);
    	if (((page * 3) + 1) < MENUMSGS) {
    		lcdPutString(menuMessages[(page * 3) + 1]);
    	} else {
    		lcdPutString("\n");
    	}
    	if (((page * 3) + 2) < MENUMSGS) {
        	lcdPutString(menuMessages[(page * 3) + 2]);
    	} else {
    		lcdPutString("\n");
    	}
        lcdPutString("\n0) Next page"); // Goes to page 0 if last page

        // wait for input, react accordingly
        while ((c = lcdGetChar()) == 0);

        if (c == '0') {
        	// Next page requested
        	if ((MENUMSGS / 3) > page) { // There are more entries
        		page++;
        	} else {
        		page = 0;
        	}
        } else if ((c != '?') && (c != '*') && (c != '#')) {
        	c -= '0';
        	if (((page * 3) + 2) > 9) {
        		// the number the user has to enter is smaller
        		temp = (page * 3) + 2;
        		while (temp > 9) {
        			temp -= 10;
        			i++;
        		}
        		c += (i * 10);
        	}
        	if (c < MENUMSGS)
        		if (menuFunctions[c] != NULL)
        			(*menuFunctions[c])();
        }
        
    }
}

void menuDriving(void) {
    uint8_t c;
    uint8_t dir;
    uint16_t dist;

	while(1) {

hell:
    	lcdPutString("\nDirection: 2 4 6 8");
    	while ((c = lcdGetChar()) == 0);
    	switch(c - '0') {
    	case 2:
    		lcdPutString("\nForwards");
    		dir = FORWARD;
    		break;
    	case 4:
    	    lcdPutString("\nLeft");
    	    dir = LEFT;
    	    break;
    	case 6:
    	    lcdPutString("\nRight");
    	    dir = RIGHT;
    	    break;
    	case 8:
    	    lcdPutString("\nBackwards");
    	    dir = BACKWARD;
    	    break;
    	case 0:
    		return;

    	default:
    		goto hell;
    	}

    	if ((dir == RIGHT) || (dir == LEFT)) {
    		lcdPutString("\nDegrees? ");
    	} else {
    		lcdPutString("\nDistance? ");
    	}
    	dist = lcdGetNum();
    	lcdPutString(bytesToString(dist));

    	if ((dir == RIGHT) || (dir == LEFT)) {
    		turn(dist, dir);
    	} else {
    		drive(dist, 200, dir);
    	}
    }
}

void menuCamera(void) {
	while(1) {
	    	lcdPutString("\nCamera...");
	    	if (lcdGetChar() != 0) {
	    		break;
	    	}
	    }
}

void menuBluetooth(void) {
	while(1) {
	    	lcdPutString("\nBluetooth...");
	    	if (lcdGetChar() != 0) {
	    		break;
	    	}
	    }
}
