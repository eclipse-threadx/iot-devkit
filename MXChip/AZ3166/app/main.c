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

#include <stdio.h>

#include "tx_api.h"

#include "board_init.h"
#include "cmsis_utils.h"
#include "screen.h"
#include "sntp_client.h"
#include "telemetry.h"
#include "wwd_networking.h"

#include "cloud_config.h"

#define ECLIPSETX_THREAD_STACK_SIZE 4096
#define ECLIPSETX_THREAD_PRIORITY   4

TX_THREAD telemetry_thread;
ULONG telemetry_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];


void tx_application_define(void* first_unused_memory)
{
    systick_interval_set(TX_TIMER_TICKS_PER_SECOND);

    // Create Telemetry thread
    UINT status = tx_thread_create(&telemetry_thread,
        "Eclipse ThreadX telemetry Thread",
        telemetry_thread_entry,
        0,
        telemetry_thread_stack,
        ECLIPSETX_THREAD_STACK_SIZE,
        ECLIPSETX_THREAD_PRIORITY,
        ECLIPSETX_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        printf("ERROR: Eclipse ThreadX telemetry thread creation failed\r\n");
    }
}

int main(void)
{
    // Initialize the board
    board_init();

    // Enter the ThreadX kernel
    tx_kernel_enter();

    return 0;
}
