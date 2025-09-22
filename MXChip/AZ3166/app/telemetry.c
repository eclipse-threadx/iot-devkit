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
#include "nanoprintf.h" 
#include "sensor.h"
#include "telemetry.h"
#include <stdio.h>

// Refresh interval
static const int32_t telemetry_interval = 5;

// Current data
static sensor_data current_sensor_data;

// Telemetry output
static const int TELEMETRY_ROWS        = 5;
static const int TELEMETRY_ROW_SIZE    = 40;
const int TELEMETRY_BUFFER_SIZE = 256;

/* Function to compare two float arrays
 * Returns true if arrays are equal within the given tolerance, otherwise false.
 */
static UINT compare_float_arrays(const float* arr1, const float* arr2, size_t size, float epsilon) {
    if (arr1 == NULL || arr2 == NULL) {
        return 0;
    }

    for (size_t i = 0; i < size; ++i) {
        if (fabsf((float)(arr1[i] - arr2[i])) > epsilon) {
            return 0; // Found a difference outside the tolerance
        }
    }
    return 1; // No significant differences found
}

/**
 * Check if the data set changed between readings.
 * 
 * Proper epsilon values for physical sensor readings depend on the sensor's accuracy. 
 * We picked a margin of error that makes sense for the use case.
 */
static UINT data_changed(sensor_data const * const current_data, sensor_data const * const new_data){
    return !(current_data->pressure_hPa == new_data->pressure_hPa &&
             current_data->temperature_degC == new_data->temperature_degC &&
             current_data->humidity_perc == new_data->humidity_perc &&
             compare_float_arrays(current_data->acceleration_mg, new_data->acceleration_mg, 3, 0.1f) &&
             compare_float_arrays(current_data->magnetic_mG, new_data->magnetic_mG, 3, 1.0f)); // Could also be 5.0f
}

static void get_sensor_data_buffer(sensor_data data, char* output){
    char buf[TELEMETRY_ROWS][TELEMETRY_ROW_SIZE];
    npf_snprintf(buf[0], TELEMETRY_ROW_SIZE, "Pressure: %.2f\r\n", (double)data.pressure_hPa);
    npf_snprintf(buf[1], TELEMETRY_ROW_SIZE, "Temperature: %.2f\r\n", (double)data.temperature_degC);
    npf_snprintf(buf[2], TELEMETRY_ROW_SIZE, "Humidity: %.2f\r\n", (double)data.humidity_perc);
    npf_snprintf(buf[3], TELEMETRY_ROW_SIZE, "Acceleration: %.2f, %.2f, %.2f\r\n", 
                                                    (double)data.acceleration_mg[0],
                                                    (double)data.acceleration_mg[1],
                                                    (double)data.acceleration_mg[2]);
    npf_snprintf(buf[4], TELEMETRY_ROW_SIZE, "Magnetic: %.2f, %.2f, %.2f\r\n", 
                                                (double)data.magnetic_mG[0],
                                                (double)data.magnetic_mG[1],
                                                (double)data.magnetic_mG[2]);

    // Initialize the new array with an empty string.
    output[0] = '\0';

    // Concatenate the strings from the 2D array.
    for (int i = 0; i < TELEMETRY_ROWS; i++) {
        strcat(output, buf[i]);
    }
}

/** 
 * Only used if LOG_TELEMETRY is defined. 
 * 
 * Uncomment the definition in telemetry.h if needed.
 */
#ifdef LOG_TELEMETRY
static void print_sensor_data(sensor_data data){
    char data_string[TELEMETRY_BUFFER_SIZE];
    get_sensor_data_buffer(data, data_string);
    printf("=====\r\n");
    printf("%s", data_string);   
    printf("=====\r\n\r\n");
}
#endif

/**
 * Entry point for the telemetry thread.
 */
void telemetry_thread_entry(ULONG parameter)
{
    //UINT status;
    sensor_data new_sensor_data;

    printf("Starting telemetry thread\r\n\r\n");

    while(1){

        // Acquire fresh data.
        lps22hb_t lps22hb_data = lps22hb_data_read();
        new_sensor_data.temperature_degC = lps22hb_data.temperature_degC;
        new_sensor_data.pressure_hPa = lps22hb_data.pressure_hPa;
        hts221_data_t hts221_data = hts221_data_read();
        new_sensor_data.humidity_perc = hts221_data.humidity_perc;
        lsm6dsl_data_t lsm6dsl_data = lsm6dsl_data_read();
        memcpy(new_sensor_data.acceleration_mg, 
               lsm6dsl_data.acceleration_mg,
               sizeof(lsm6dsl_data.acceleration_mg));
        lis2mdl_data_t lis2mdl_data = lis2mdl_data_read();
        memcpy(new_sensor_data.magnetic_mG, 
               lis2mdl_data.magnetic_mG,
               sizeof(lis2mdl_data.magnetic_mG));

        if (data_changed(&current_sensor_data, &new_sensor_data)){
            #ifdef LOG_TELEMETRY
                printf("Telemetry changed.\r\n");
                print_sensor_data(new_sensor_data);
            #endif
            tx_event_flags_set(&mqtt_app_flag, MQTT_MESSAGE_READY, TX_OR);
            current_sensor_data = new_sensor_data;
        }
        #ifdef LOG_TELEMETRY
        else{
            printf("Telemetry did not change.\r\n");
        }
        #endif

        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * telemetry_interval);
    }
}

/**
 * Returns current telemetry as a string.
 */
void get_current_telemetry_string(char* output){
    get_sensor_data_buffer(current_sensor_data, output);
}
