/*
 * motor.h
 *
 *  Created on: 17.07.2011
 *      Author: thomas
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define FORWARD 1
#define BACKWARD 2
#define TURNRIGHT 3
#define TURNLEFT 4

void motorInit(void);
void motorSpeed(uint8_t left, uint8_t right);
void motorStop(void);
void motorDirection(uint8_t dir);

#endif /* MOTOR_H_ */
