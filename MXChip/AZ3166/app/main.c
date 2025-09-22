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
 *     Frédéric Desbiens - Added telemetry and MQTT.
 */

#include <stdio.h>

#include "tx_api.h"

#include "board_init.h"
#include "cmsis_utils.h"
#include "mqtt_client.h"
#include "screen.h"
#include "sntp_client.h"
#include "telemetry.h"
#include "wwd_networking.h"

#include "cloud_config.h"

#define ECLIPSETX_THREAD_STACK_SIZE 4096
#define ECLIPSETX_THREAD_PRIORITY   4
#define BYTE_POOL_SIZE              1024
#define QUEUE_SIZE                  16


TX_QUEUE mqtt_queue;
TX_EVENT_FLAGS_GROUP mqtt_app_flag;
TX_BYTE_POOL byte_pool;
TX_THREAD telemetry_thread;
TX_THREAD mqtt_thread;
ULONG telemetry_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];
ULONG mqtt_thread_stack[ECLIPSETX_THREAD_STACK_SIZE / sizeof(ULONG)];
TX_EVENT_FLAGS_GROUP mqtt_app_flag;

void tx_application_define(void* first_unused_memory)
{
    systick_interval_set(TX_TIMER_TICKS_PER_SECOND);
    CHAR *pointer;

    /* Allocate the message queue. */
    tx_byte_allocate(&byte_pool, (VOID **)&pointer, QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);

    /* Create the message queue shared by the telemetry and MQTT threads */
    tx_queue_create(&mqtt_queue, "Shared queue", TX_1_ULONG, pointer, QUEUE_SIZE*sizeof(ULONG));

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

    // Create MQTT thread
    status = tx_thread_create(&mqtt_thread,
        "Eclipse ThreadX MQTT Thread",
        mqtt_thread_entry,
        0,
        mqtt_thread_stack,
        ECLIPSETX_THREAD_STACK_SIZE,
        ECLIPSETX_THREAD_PRIORITY,
        ECLIPSETX_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START); 

    if (status != TX_SUCCESS)
    {
        printf("ERROR: Eclipse ThreadX MQTT thread creation failed\r\n");
    }}

int main(void)
{
    // Initialize the board
    board_init();

    // Enter the ThreadX kernel
    tx_kernel_enter();

    return 0;
}
