//
//  snippets.c
//  Robot
//
//  Created by Thomas Buck on 28.07.11.
//  Copyright 2011 Claude-Dornier-Schule. All rights reserved.
//


int remote(void) {
    
	char c;
	char buf[5];
	uint8_t speed = 0, dir = FORWARD;
    
	twiInit();
	ledInit();
	serialInit(51, 8, NONE, 1);
	motorInit();
    
	sei();
    
	ledToggle(0);
	ledToggle(1);
	ledToggle(2);
	_delay_ms(1000);
	ledToggle(0);
	ledToggle(1);
	ledToggle(2);
	_delay_ms(1000);
	ledToggle(0);
	ledToggle(1);
	ledToggle(2);
	_delay_ms(1000);
	serialWriteString("Drive Test...\r\n");
	ledToggle(0);
	ledToggle(1);
	ledToggle(2);
    
	motorDirection(dir);
	motorSpeed(speed, speed);
    
	while(1) {
		if (serialHasChar() == 1) {
			c = serialGet();
			if ((c == 'w') || (c == '2')) {
				if (speed <= 205) {
					speed += 50;
				}
				dir = FORWARD;
			}
			if ((c == 's') || (c == '8')) {
				motorStop();
				speed = 0;
			}
			if ((c == ' ') || (c == '5')) {
				if (dir == FORWARD) {
					dir = BACKWARD;
				} else if (dir == BACKWARD) {
					dir = FORWARD;
				} else if (dir == TURNRIGHT) {
					dir = TURNLEFT;
				} else if (dir == TURNLEFT) {
					dir = TURNRIGHT;
				}
			}
			if ((c == 'd') || (c == '6')) {
				dir = TURNRIGHT;
				speed = 100;
			}
			if ((c == 'a') || (c == '4')) {
				dir = TURNLEFT;
				speed = 100;
			}
            
            
			motorDirection(dir);
			motorSpeed(speed, speed);
			serialWriteString(utoa(speed, buf, 10));
			serialWrite(' ');
			serialWrite(c);
			serialWrite('\n');
		}
	}
    
	return 0;
}