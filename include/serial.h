/*
 * uart.h
 *
 *  Created on: 09.07.2011
 *      Author: thomas
 */

#ifndef UART_H_
#define UART_H_

// RX & TX buffer size in bytes (power of 2)
#define RX_BUFFER_SIZE 512
#define TX_BUFFER_SIZE 512

// Select Baudrate with this macro
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

#define ODD 2
#define EVEN 1
#define NONE 0

uint8_t serialInit(uint16_t baud, uint8_t databits, uint8_t parity, uint8_t stopbits);

uint8_t serialHasChar(void);

uint8_t serialGet(void);

void serialWrite(uint8_t data);
void serialWriteString(char *data);

void serialClose(void);

#endif /* UART_H_ */
