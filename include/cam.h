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

#define CAMDELAY 2000 // in microseconds

void camInit(uint8_t *regs);
void camShoot(void);
uint8_t camReady(void);
uint8_t *camGetPicture(void);

#endif
