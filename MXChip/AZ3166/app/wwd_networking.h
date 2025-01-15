/* 
 * Copyright (c) Microsoft
 * Copyright (c) 2024 Eclipse Foundation
 * 
 *  This program and the accompanying materials are made available 
 *  under the terms of the MIT license which is available at
 *  https://opensource.org/license/mit.
 * 
 *  SPDX-License-Identifier: MIT
 * 
 *  Contributors: 
 *     Microsoft         - Initial version
 *     Frédéric Desbiens - 2024 version.
 */

#ifndef _WWD_NETWORKING_H
#define _WWD_NETWORKING_H

#include "nx_api.h"
#include "nxd_dns.h"

#include "cloud_config.h"

extern NX_PACKET_POOL nx_pool[2]; // 0=TX, 1=RX.
extern NX_IP nx_ip;
extern NX_DNS nx_dns_client;

UINT wwd_network_init(CHAR* ssid, CHAR* password, WiFi_Mode mode);
UINT wwd_network_connect();

#endif
