/**
 * @file handlenetwork.cpp
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

#include "nextion.h"

#include "network.h"

#include "debug.h"

void Nextion::SetIp(const char *pObjectName, uint32_t nIp) {
	char componentText[64];

	snprintf(componentText, sizeof componentText - 1, "%s.txt=\"" IPSTR "\"", pObjectName, IP2STR(nIp));
	SendCommand(componentText);
}

void Nextion::HandleNetworkGet(void) {
	DEBUG2_ENTRY

	SetIp("n_ip", Network::Get()->GetIp());
	SetIp("n_netmask", Network::Get()->GetNetmask());
	SetIp("n_gw", Network::Get()->GetGatewayIp());
	SetText("n_hostname", Network::Get()->GetHostName());
	SetValue("n_dhcp", static_cast<uint32_t>(Network::Get()->IsDhcpUsed()));

	DEBUG2_EXIT
}

void Nextion::HandleNetworkSave(void) {
	DEBUG2_ENTRY

	DEBUG2_EXIT
}
