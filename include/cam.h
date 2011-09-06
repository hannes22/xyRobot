//
//  cam.h
//  Robot
//
//  Created by Thomas Buck on 28.07.11.
//  Copyright 2011 Claude-Dornier-Schule. All rights reserved.
//

#ifndef Robot_cam_h
#define Robot_cam_h

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

#define CAMDELAY 1 // in microseconds

// To get debugging messages, define CAMDEBUG
// Set print handler in cam.c

uint8_t *camInit(uint8_t *regs);
uint8_t *camShoot(void);
void camReset(void);

#endif
