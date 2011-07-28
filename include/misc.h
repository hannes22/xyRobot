/*
 * misc.h
 *
 *  Created on: 12.07.2011
 *      Author: thomas
 */

#ifndef MISC_H_
#define MISC_H_

#define LCD_ADDRESS 0x66
#define LINE_BUFFER_LENGTH 60

char lineBuffer[LINE_BUFFER_LENGTH];

void ledInit(void);
void ledToggle(uint8_t id);
void ledSet(uint8_t id, uint8_t val);

void lcdPutChar(char c);
void lcdPutString(char* s);
uint8_t lcdGetChar(void);
uint16_t lcdGetNum(void);

uint8_t serialGetLine(uint8_t block);

char *byteToString(uint8_t byte);

#endif /* MISC_H_ */
