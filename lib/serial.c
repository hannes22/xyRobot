/*
 * uart.c
 *
 *  Created on: 09.07.2011
 *      Author: thomas
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "include/serial.h"

uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t txBuffer[TX_BUFFER_SIZE];
uint16_t rxHead = 0;
uint16_t rxTail = 0;
uint16_t txHead = 0;
uint16_t txTail = 0;
uint8_t shouldStartTransmission = 0;

ISR(USART0_RX_vect) { // Receive complete
	rxBuffer[rxTail] = UDR0;
	if (rxTail < (RX_BUFFER_SIZE - 1)) {
		rxTail++;
	} else {
		rxTail = 0;
	}
}

ISR(USART0_UDRE_vect) { // Data register empty
	if (txHead != txTail) {
		UDR0 = txBuffer[txHead];
		if (txHead < (TX_BUFFER_SIZE -1)) {
			txHead++;
		} else {
			txHead = 0;
		}
	} else {
		shouldStartTransmission = 1;
		UCSR0B &= ~(1 << UDRIE0); // Disable Interrupt
	}
}

uint8_t serialInit(uint16_t baud, uint8_t databits, uint8_t parity, uint8_t stopbits) {
	if (parity > ODD) {
		return 1;
	}
	if ((databits < 5) || (databits > 8)) {
		return 1;
	}
	if ((stopbits < 1) || (stopbits > 2)) {
		return 1;
	}

	if (parity != NONE) {
		UCSR0C |= (1 << UPM01);
		if (parity == ODD) {
			UCSR0C |= (1 << UPM00);
		}
	}
	if (stopbits == 2) {
		UCSR0C |= (1 << USBS0);
	}
	if (databits != 5) {
		if ((databits == 6) || (databits >= 8)) {
			UCSR0C |= (1 << UCSZ00);
		}
		if (databits >= 7) {
			UCSR0C |= (1 << UCSZ01);
		}
		if (databits == 9) {
			UCSR0B |= (1 << UCSZ02);
		}
	}
	UBRR0 = baud;
	UCSR0B |= (1 << RXCIE0) | (1 << UDRIE0) | (1 << RXEN0) | (1 << TXEN0); // Enable Interrupts and Receiver/Transmitter

	return 0;
}

uint8_t serialHasChar() {
	if (rxHead != rxTail) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t serialGet() {
	uint8_t c;
	if (rxHead != rxTail) {
		c = rxBuffer[rxHead];
		rxBuffer[rxHead] = 0;
		if (rxHead < (RX_BUFFER_SIZE - 1)) {
			rxHead++;
		} else {
			rxHead = 0;
		}
		return c;
	} else {
		return 0;
	}
}

void serialWrite(uint8_t data) {
	txBuffer[txTail] = data;
	if (txTail < (TX_BUFFER_SIZE - 1)) {
		txTail++;
	} else {
		txTail = 0;
	}
	if (shouldStartTransmission == 1) {
		shouldStartTransmission = 0;
		UCSR0B |= (1 << UDRIE0); // Enable Interrupt
	}
}

void serialWriteString(char *data) {
	while (*data != '\0') {
		serialWrite(*data++);
	}
}

void serialClose() {
	UCSR0B = 0;
	UCSR0C = 0;
	UBRR0 = 0;
	rxHead = 0;
	txHead = 0;
	rxTail = 0;
	txTail = 0;
}
