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

#ifndef _SNTP_CLIENT_H
#define _SNTP_CLIENT_H

#include <tx_api.h>

ULONG sntp_time_get();
UINT sntp_time(ULONG* unix_time);

UINT sntp_init();
UINT sntp_sync();

#endif
