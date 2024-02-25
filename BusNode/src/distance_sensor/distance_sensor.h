/*
 * distance_sensor.h
 *
 *  Created on: Feb. 9, 2024
 *      Author: Joel
 */

#ifndef SRC_DISTANCE_SENSOR_DISTANCE_SENSOR_H_
#define SRC_DISTANCE_SENSOR_DISTANCE_SENSOR_H_

#include "rf_driver_hal.h"

void ds_init();
uint8_t ds_is_init();
I2C_HandleTypeDef* ds_get_i2c_handle();

void ds_tick();

#endif /* SRC_DISTANCE_SENSOR_DISTANCE_SENSOR_H_ */
