/**
 * @file sc16is750.h
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

#ifndef SC16IS750_H_
#define SC16IS750_H_

#include <stdint.h>

#include "hardware.h"
#include "sc16is7x0.h"

enum {
	SC16IS750_I2C_ADDRESS = 0x4D
};

enum {
	SC16IS750_CRISTAL_HZ = 14745600UL
};

enum TSC16IS750SerialParity {
	SC16IS750_SERIAL_PARITY_NONE,
	SC16IS750_SERIAL_PARITY_ODD,
	SC16IS750_SERIAL_PARITY_EVEN,
	SC16IS750_SERIAL_PARITY_FORCED0,
	SC16IS750_SERIAL_PARITY_FORCED1
};

class SC16IS750  {
public:
	SC16IS750(void);
	SC16IS750(uint8_t nAddress, uint32_t nOnBoardCrystal = SC16IS750_CRISTAL_HZ);
	~SC16IS750(void);

	void SetFormat(uint32_t nBits, TSC16IS750SerialParity tParity, uint32_t nStopBits);
	void SetBaud(uint32_t nBaudrate);

	bool Start(void);

	bool IsReadable(void) {
		if ((RegRead(SC16IS7X0_LSR) & LSR_DR) == LSR_DR) {
			return true;
		} else {
			return false;
		}
	}

	bool IsReadable(uint32_t nTimeOut) {
		const uint32_t nWait = Hardware::Get()->Millis() + nTimeOut;
		do {
			if ((RegRead(SC16IS7X0_LSR) & LSR_DR) == LSR_DR) {
				return true;
			}
		} while (Hardware::Get()->Millis() < nWait);

		return false;
	}

	int Getc(void) {
		if (!IsReadable()) {
			return -1;
		}

		return (int) RegRead(SC16IS7X0_RHR);
	}

	bool IsWritable(void) {
		if ((RegRead(SC16IS7X0_LSR) & LSR_THRE) == LSR_THRE) {
			return true;
		} else {
			return false;
		}
	}

	int Putc(int nValue) {
		while (RegRead(SC16IS7X0_TXLVL) == 0) {

		}

		RegWrite(SC16IS7X0_THR, nValue);

		return nValue;
	}

	int Puts(const char *s) {
		int i = 0;
		char c;

		while ((c = *s++) != (char) 0) {
			i++;
			Putc((int) c);
		}

		return i;
	}

	void RegWrite(uint8_t nRegister, uint8_t nValue);
	uint8_t RegRead(uint8_t nRegister);

private:
	void I2cSetup(void);

private:
	uint8_t m_nAddress;
	uint32_t m_nOnBoardCrystal;
};

#endif /* SC16IS750_H_ */
