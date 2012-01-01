/*
 * serial.c
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include <serial.h>

uint8_t volatile rxBuffer[RX_BUFFER_SIZE];
uint8_t volatile txBuffer[TX_BUFFER_SIZE];
uint16_t volatile rxRead = 0;
uint16_t volatile rxWrite = 0;
uint16_t volatile txRead = 0;
uint16_t volatile txWrite = 0;
uint8_t volatile shouldStartTransmission = 0;

ISR(USART0_RX_vect) { // Receive complete
    rxBuffer[rxWrite] = UDR0;
	if (rxWrite < (RX_BUFFER_SIZE - 1)) {
		rxWrite++;
	} else {
		rxWrite = 0;
	}
}

ISR(USART0_UDRE_vect) { // Data register empty
    if (txRead != txWrite) {
		UDR0 = txBuffer[txRead];
		if (txRead < (TX_BUFFER_SIZE -1)) {
			txRead++;
		} else {
			txRead = 0;
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
	UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // Enable Interrupts and Receiver/Transmitter

	return 0;
}

uint8_t serialHasChar() {
	if (rxRead != rxWrite) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t serialGet() {
	uint8_t c;
	if (rxRead != rxWrite) {
		c = rxBuffer[rxRead];
		rxBuffer[rxRead] = 0;
		if (rxRead < (RX_BUFFER_SIZE - 1)) {
			rxRead++;
		} else {
			rxRead = 0;
		}
		return c;
	} else {
		return 0;
	}
}

void serialWrite(uint8_t data) {
	while (((txWrite + 1) == txRead) || ((txRead == 0) && ((txWrite + 1) == TX_BUFFER_SIZE))); // Buffer is full, wait!
    txBuffer[txWrite] = data;
	if (txWrite < (TX_BUFFER_SIZE - 1)) {
		txWrite++;
	} else {
		txWrite = 0;
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
	rxRead = 0;
	txRead = 0;
	rxWrite = 0;
	txWrite = 0;
}
