/**
 * @file sc16is750.cpp
 *
 */
/* Copyright (C) 2019 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <sc16is750.h>

#include "sc16is7x0.h"

#include "i2c.h"

SC16IS750::SC16IS750(void) {
	m_nAddress = SC16IS750_I2C_ADDRESS;
	m_nOnBoardCrystal = SC16IS750_CRISTAL_HZ;
}

SC16IS750::SC16IS750(uint8_t nAddress, uint32_t nOnBoardCrystal): m_nAddress(nAddress), m_nOnBoardCrystal(nOnBoardCrystal) {
}

SC16IS750::~SC16IS750(void) {
}

bool SC16IS750::Start(void) {
	i2c_begin();

	I2cSetup();

	if (!i2c_is_connected(m_nAddress)) {
		return false;
	}

	SetFormat(8, SC16IS750_SERIAL_PARITY_NONE, 1);
	SetBaud(SC16IS7X0_DEFAULT_BAUDRATE);

	RegWrite(SC16IS7X0_FCR, (FCR_ENABLE_FIFO | FCR_RX_FIFO_RST | FCR_TX_FIFO_RST));

	const uint8_t TEST_CHARACTER = 'A';

	RegWrite(SC16IS7X0_SPR, TEST_CHARACTER);

	return (RegRead(SC16IS7X0_SPR) == TEST_CHARACTER);
}

void SC16IS750::SetFormat(uint32_t nBits, TSC16IS750SerialParity tParity,  uint32_t nStopBits) {
	uint8_t lcr = 0x00;

	switch (nBits) {
	case 5:
		lcr |= LCR_BITS5;
		break;
	case 6:
		lcr |= LCR_BITS6;
		break;
	case 7:
		lcr |= LCR_BITS7;
		break;
	case 8:
		lcr |= LCR_BITS8;
		break;
	default:
		lcr |= LCR_BITS8;
	}

	switch (tParity) {
	case SERIAL_PARITY_NONE:
		lcr |= LCR_NONE;
		break;
	case SERIAL_PARITY_ODD:
		lcr |= LCR_ODD;
		break;
	case SERIAL_PARITY_EVEN:
		lcr |= LCR_EVEN;
		break;
	case SERIAL_PARITY_FORCED1:
		lcr |= LCR_FORCED1;
		break;
	case SERIAL_PARITY_FORCED0:
		lcr |= LCR_FORCED0;
		break;
	default:
		lcr |= LCR_NONE;
	}

	switch (nStopBits) {
	case 1:
		lcr |= LCR_BITS1;
		break;
	case 2:
		lcr |= LCR_BITS2;
		break;
	default:
		lcr |= LCR_BITS1;
	}

	RegWrite(SC16IS7X0_LCR, lcr);
}

void SC16IS750::SetBaud(uint32_t nBaudrate) {
#define SC16IS750_PRESCALER_1               1			///< Default prescaler after reset
#define SC16IS750_PRESCALER                 SC16IS750_PRESCALER_1
#define SC16IS750_BAUDRATE_DIVISOR(baud)	((m_nOnBoardCrystal/SC16IS750_PRESCALER)/(baud*16UL))

	const uint32_t nDivisor = (uint32_t) SC16IS750_BAUDRATE_DIVISOR(nBaudrate);
	const uint8_t lcr = RegRead(SC16IS7X0_LCR);

	RegWrite(SC16IS7X0_LCR, lcr | LCR_ENABLE_DIV);
	RegWrite(SC16IS7X0_DLL, (nDivisor & 0xFF));
	RegWrite(SC16IS7X0_DLH, ((nDivisor >> 8) & 0xFF));
	RegWrite(SC16IS7X0_LCR, lcr);
}

void SC16IS750::RegWrite(uint8_t nRegister, uint8_t nValue) {
	I2cSetup();
	i2c_write_reg_uint8(nRegister << 3, nValue);
}

uint8_t SC16IS750::RegRead(uint8_t nRegister) {
	I2cSetup();
	return i2c_read_reg_uint8(nRegister << 3);
}

void SC16IS750::I2cSetup(void) {
	i2c_set_baudrate(I2C_FULL_SPEED);
	i2c_set_address(m_nAddress);
}
