/* 
 *  Copyright (c) 2025 Eclipse Foundation
 * 
 *  This program and the accompanying materials are made available 
 *  under the terms of the MIT license which is available at
 *  https://opensource.org/license/mit.
 * 
 *  SPDX-License-Identifier: MIT
 * 
 *  Contributors: 
 *     Frédéric Desbiens - Initial version.
 */
#include "cloud_config.h"
#include "mqtt_client.h"
#include "nx_api.h"
#include "telemetry.h"
#include "wwd_networking.h"

#include <stdint.h>
#include <string.h>

// Helper function.
#define STRLEN(p) (sizeof(p) - 1)

/* Declare the MQTT thread stack space. */
static ULONG mqtt_client_stack[MQTT_CLIENT_STACK_SIZE / sizeof(ULONG)];

/* Declare buffers to hold message and topic. */
static UCHAR message_buffer[NXD_MQTT_MAX_MESSAGE_LENGTH];
static UCHAR topic_buffer[NXD_MQTT_MAX_TOPIC_NAME_LENGTH];


/* Declare the MQTT client control block. */
static NXD_MQTT_CLIENT mqtt_client;


/* Declare the disconnect notify function. */
static VOID client_disconnect_func(NXD_MQTT_CLIENT *client_ptr)
{
    NX_PARAMETER_NOT_USED(client_ptr);
    printf("client disconnected from broker.\r\n");
}

static void send_message(){
    UINT status;
    
    /* Publish a message with QoS Level 1. */
    char buffer[160] = {0};
    get_current_telemetry_string(buffer);
    //printf("%s", buffer);

    status = nxd_mqtt_client_publish(&mqtt_client, MQTT_PUBLISH_TOPIC, STRLEN(MQTT_PUBLISH_TOPIC),
                                        (CHAR *)buffer, strlen(buffer), 0, QOS1, NX_WAIT_FOREVER);

    if (status != NXD_MQTT_SUCCESS){
        printf("Publish failed with code: %d\r\n", status);
    }
    else{
        printf("Published message.\r\n");
    }
}

static void receive_message(){
    UINT status;
    UINT topic_length, message_length;
    ULONG message_sent = 0;

    status = nxd_mqtt_client_message_get(&mqtt_client, topic_buffer, sizeof(topic_buffer), &topic_length,
                                        message_buffer, sizeof(message_buffer), &message_length);
    printf("Received message and status: %d \r\n", status);
    if (status == NXD_MQTT_SUCCESS){
        topic_buffer[topic_length] = 0;
        message_buffer[message_length] = 0;
        message_sent = message_buffer[0];
        status = tx_queue_send(&mqtt_queue, &message_sent, TX_WAIT_FOREVER);
        printf("Topic: %s, Message: %s\r\n", topic_buffer, message_buffer);
    }
}

static VOID client_notify_func(NXD_MQTT_CLIENT *client_ptr, UINT number_of_messages)
{
    NX_PARAMETER_NOT_USED(client_ptr);
    NX_PARAMETER_NOT_USED(number_of_messages);
    tx_event_flags_set(&mqtt_app_flag, MQTT_RECEIVE_EVENT, TX_OR);
    return;
}

static ULONG error_count;
static void mqtt_thread_work(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr){
    UINT status;
    NXD_ADDRESS server_ip;
    ULONG events;

    printf("Creating MQTT client\r\n");
    /* Create MQTT client instance. */
    status = nxd_mqtt_client_create(&mqtt_client, MQTT_CLIENT_NAME, MQTT_CLIENT_NAME, STRLEN(MQTT_CLIENT_NAME),
                                    ip_ptr, pool_ptr, (VOID *)mqtt_client_stack, sizeof(mqtt_client_stack),
                                    MQTT_THREAD_PRIORTY, NX_NULL, 0);

    if (status){
        printf("Error in creating MQTT client: 0x%02x\n", status);
        error_count++;
    }

    printf(" MQTT client created\r\n");

    /* Register the disconnect notification function. */
    nxd_mqtt_client_disconnect_notify_set(&mqtt_client, client_disconnect_func);

    /* Create an event flag for this demo. */
    status = tx_event_flags_create(&mqtt_app_flag, "MQTT event");
    if (status)
        error_count++;

    server_ip.nxd_ip_version = 4;
    server_ip.nxd_ip_address.v4 = MQTT_LOCAL_BROKER_IP;

    /* Start the connection to the server. */

    status = nxd_mqtt_client_connect(&mqtt_client, &server_ip, NXD_MQTT_PORT,
                                     MQTT_KEEP_ALIVE_TIMER, 0, NX_WAIT_FOREVER);
    if (status != NXD_MQTT_SUCCESS){
                printf("MQTT connect failed with code: %d\r\n", status);
    }
    else{
        printf("MQTT Client connected.\r\n");
    }

    /* Subscribe to the topic with QoS level 0. */
    status = nxd_mqtt_client_subscribe(&mqtt_client, MQTT_SUBSCRIBE_TOPIC, STRLEN(MQTT_SUBSCRIBE_TOPIC), QOS0);
    if (status != NXD_MQTT_SUCCESS){
                printf("MQTT subscribe failed with code: %d\r\n", status);
    }
    else{
        printf("Subscribed to topic %s.\r\n", MQTT_SUBSCRIBE_TOPIC);
    }

    /* Set the receive notify function. */
    status = nxd_mqtt_client_receive_notify_set(&mqtt_client, client_notify_func);
    if (status != NXD_MQTT_SUCCESS){
                printf("MQTT receive notify setup failed with code: %d\r\n", status);
    }
    else{
        printf("MQTT Receive notify function set.\r\n");
    }

    /* Now wait for the broker to publish the message. */
    printf("Waiting for messages\r\n");

    while (1){
        tx_event_flags_get(&mqtt_app_flag, MQTT_ALL_EVENTS, TX_OR_CLEAR, &events, TX_WAIT_FOREVER);
        if (events & MQTT_RECEIVE_EVENT){
            receive_message();

        }
        else if (events & MQTT_MESSAGE_READY){
            send_message();
        }
    }

    /* Cleanup. Release resources. */
    nxd_mqtt_client_unsubscribe(&mqtt_client, MQTT_SUBSCRIBE_TOPIC, STRLEN(MQTT_SUBSCRIBE_TOPIC));
    nxd_mqtt_client_disconnect(&mqtt_client);
    nxd_mqtt_client_delete(&mqtt_client);

    return;
}

void mqtt_thread_entry(ULONG parameter){

    UINT status;

    printf("Starting Eclipse ThreadX MQTT thread\r\n\r\n");

    // Initialize the network
    if ((status = wwd_network_init(WIFI_SSID, WIFI_PASSWORD, WIFI_MODE))){
        printf("ERROR: Failed to initialize the network (0x%08x)\r\n", status);
    }

    wwd_network_connect();

    mqtt_thread_work(&nx_ip, nx_pool);
}
