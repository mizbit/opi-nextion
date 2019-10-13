/**
 * @file nextion.cpp
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
#include <stdio.h>
#include <string.h>
#ifndef NDEBUG	// TODO remove
 #include <ctype.h>
#endif
#include <assert.h>

#include "nextion.h"

#include "hardware.h"

#include "debug.h"

#ifndef ALIGNED
 #define ALIGNED __attribute__ ((aligned (4)))
#endif

static const uint8_t s_aTermination[4] ALIGNED = { 0xFF, 0xFF, 0xFF, 0x00};

static const uint32_t s_aValidBaud[] = {2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 256000, 512000, 921600};
#define ARRAY_SIZE	((sizeof s_aValidBaud) / (sizeof s_aValidBaud[0]))

enum TReturnCodes {
	RETURN_CODE_INVALID_VARIABLE_NAME = 0x1A,
	RETURN_CODE_TOUCH_EVENT = 0x65,
	RETURN_CODE_READY = 0x88
};

Nextion::Nextion(void): m_nBaud(9600) {
}

Nextion::~Nextion(void) {
}

void Nextion::SetBaud(uint32_t nBaud) {
	DEBUG_ENTRY

	for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
		if (nBaud == s_aValidBaud[i]) {
			m_nBaud = nBaud;

			DEBUG_EXIT
			return;
		}
	}

	m_nBaud = 9600;

	DEBUG_EXIT
	return;
}

bool Nextion::Start(void) {
	DEBUG_ENTRY

	if (!SC16IS750::Start()) {
		DEBUG_EXIT
		return false;
	}

	SC16IS750::SetBaud(m_nBaud);

	SendCommand("bkcmd=1");
	const bool r1 = ReceiveCommandResponse();

	SendCommand("page 0");
	const bool r2 = ReceiveCommandResponse();

	SendCommand("bkcmd=2");

	DEBUG_PRINTF("%d %d", (int ) r1, (int ) r2);
	DEBUG_EXIT

	return r1 & r2;
}

bool Nextion::ReceiveCommandResponse(void) {
	int c;
	uint32_t nCount0xFF = 0;
	uint32_t nCount = 0;

	while ((c = GetChar(100)) != -1) {
		if (c == 0xFF) {
			nCount0xFF++;
		} else {
			m_aCommandReturned[nCount++] = c;
		}

		if (nCount0xFF == 3) {
			break;
		}

		if (nCount > 1) {
			break;
		}
	}

	DEBUG_PRINTF("%.2x %.2x %.2x ", m_aCommandReturned[0], nCount0xFF, nCount);

	 return (m_aCommandReturned[0] == 0x01) && (nCount0xFF == 3) && (nCount == 1);
}


void Nextion::Run(void) {
	Listen();
}

bool Nextion::Listen(void) {
#define TO_HEX(i)	((i) < 10) ? '0' + (i) : 'A' + ((i) - 10)

	int c;
	uint32_t nCount0xFF = 0;
	m_nCount = 0;

	while ((c = GetChar(100)) != -1) {
		DEBUG_PRINTF("%.2x [%c]", (int) c, isprint(c) ? c : '.');

		if ((m_nCount == 0) && (c == 0)) {
			return false;
		}

		if (c == 0xFF) {
			nCount0xFF++;
		} else {
			m_aCommandReturned[m_nCount++] = c;
		}

		if (nCount0xFF == 3) {
			break;
		}
	}

	if (nCount0xFF == 3) {
		switch (m_aCommandReturned[0]) {
#ifndef NDEBUG
		case RETURN_CODE_INVALID_VARIABLE_NAME:
			DEBUG_PUTS("!> Invalid Variable name or attribute <!");
			break;
#endif
		case RETURN_CODE_TOUCH_EVENT:
			HandleTouchEvent();
			break;
		case RETURN_CODE_READY:
			HandleReady();
			break;
		default:
			break;
		}
	}

	if (m_nCount > 0) {

		HandleRawData();
	}

	return (nCount0xFF == 3);
}

void Nextion::SendCommand(const char *pCommand) {
	assert(pCommand != 0);

	DEBUG_PUTS(pCommand);

	while (GetChar() != -1)
		;

	Puts(pCommand);
	Puts((const char *)s_aTermination);
}

void Nextion::SetText(const char *pObjectName, const uint8_t *pValue) {
	char componentText[80];

	int i __attribute__((unused));

	i = snprintf(componentText, sizeof componentText - 1, "%s.txt=\"%s\"", pObjectName, pValue);

	DEBUG_PRINTF("i=%d", i);

	SendCommand(componentText);
}

uint32_t Nextion::GetText(const char *pObjectName, uint8_t *pValue, uint32_t nLength) {
	return 0; //TODO GetText
}

void Nextion::SetValue(const char *pObjectName, uint32_t nValue) {
	char componentText[32];

	snprintf(componentText, sizeof componentText - 1, "%s.val=%d", pObjectName, nValue);
	SendCommand(componentText);
}

bool Nextion::GetValue(const char *pObjectName, uint32_t &nValue) {
	char componentText[32];

	snprintf(componentText, sizeof componentText - 1, "get %s.val", pObjectName);
	SendCommand(componentText);

	return ReceiveReturnedValue(nValue);
}

bool Nextion::ReceiveReturnedValue(uint32_t &nValue) {
	DEBUG2_ENTRY

	int c;
	uint32_t nCount0xFF = 0;
	uint32_t nCount = 0;

	while ((c = GetChar(100)) != -1) {
		DEBUG_PRINTF("%.2x [%c]", (int) c, isprint(c) ? c : '.');

		if (nCount > 5) {
			DEBUG2_EXIT
			return false;;
		}

		if (c == 0xFF) {
			nCount0xFF++;
		} else {
			m_aCommandReturned[nCount++] = c;
		}

		if (nCount0xFF == 3) {
			if (nCount != 5) {
				DEBUG2_EXIT
				return false;
			}
			break;
		}
	}

	if (m_aCommandReturned[0] != 0x71) {
		DEBUG2_EXIT
		return false;
	}

	nValue = ((uint32_t)m_aCommandReturned[4] << 24) | ((uint32_t)m_aCommandReturned[3] << 16) | (m_aCommandReturned[2] << 8) | (m_aCommandReturned[1]);

	DEBUG_PRINTF("nValue=%d [%x]", nValue, nValue);

	DEBUG2_EXIT
	return true;
}

void Nextion::HandleTouchEvent(void) {
	DEBUG_ENTRY

#ifndef NDEBUG
	const uint32_t nPageNumber = m_aCommandReturned[1];
	const uint32_t nComponentId = m_aCommandReturned[2];
	const uint32_t nEvent = m_aCommandReturned[3];

	DEBUG_PRINTF("Page number=%d, Component Id=%d, Event=%s", nPageNumber, nComponentId, nEvent == 0x00 ? "Release" : "Press");
#endif

	DEBUG_EXIT
}

void Nextion::HandleReady(void) {
	DEBUG_ENTRY

	DEBUG_EXIT
}
