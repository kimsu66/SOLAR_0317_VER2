/*
 * bms_sensor.h
 *
 *  Created on: 2026. 3. 21.
 *      Author: kimsuyeon
 *
 *      TEMP task, GAS task, INA219(BMS 전압/전류) task
 *			각 상태값, 측정값 저장
 *			getter 제공
 */

#ifndef INC_BMS_SENSOR_H_
#define INC_BMS_SENSOR_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#include "temp.h"
#include "gas2.h"
#include "ina219_bms.h"

void BMS_SENSOR_Init(void);
void BMS_SENSOR_Task(void);

/* TEMP */
TEMP_STATE GetTempState(void);
int16_t GetTempC(void);
uint16_t GetTempADC(void);

/* GAS */
GasLevel_t GetGasState(void);
uint16_t GetGasADC(void);

/* VOLTAGE / CURRENT */
VOLTAGE_STATE GetVoltageState(void);
CURRENT_STATE GetCurrentState(void);
int32_t GetVoltagemV(void);
int32_t GetCurrentmA(void);



#endif /* INC_BMS_SENSOR_H_ */
