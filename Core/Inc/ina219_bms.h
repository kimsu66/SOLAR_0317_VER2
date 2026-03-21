/*
 * ina219_bms.h
 *
 *  Created on: 2026. 3. 18.
 *      Author: kimsuyeon
 *
 *      [Main role]
 *
 *      INA219 초기화
 *      전압 / 전류 읽기
 *      SAFE / WARNING / DANGER 상태 판정
 *      현재 상태 반환
 */

#ifndef INC_INA219_BMS_H_
#define INC_INA219_BMS_H_

#include "main.h"
#include <stdint.h>
#include "i2c.h"

typedef enum {
		CURRENT_STATE_SAFE = 0,
		CURRENT_STATE_WARNING,
		CURRENT_STATE_DANGER
} CURRENT_STATE;

typedef enum {
    VOLTAGE_STATE_SAFE = 0,
		VOLTAGE_STATE_OVER_WARNING,
		VOLTAGE_STATE_OVER_DANGER,
		VOLTAGE_STATE_UNDER_WARNING,
		VOLTAGE_STATE_UNDER_DANGER
} VOLTAGE_STATE;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t addr;
    int32_t rshunt_mohm;       // shunt resistor in mΩ
} INA219_BMS_t;

uint8_t INA219_BMS_Init(void);

int32_t INA219_BMS_ReadVoltage_mV(void);
int32_t INA219_BMS_ReadCurrent_mA(void);

CURRENT_STATE INA219_BMS_GetCurrentState(int32_t current_mA);
VOLTAGE_STATE INA219_BMS_GetVoltageState(int32_t voltage_mV);


#endif /* INC_INA219_BMS_H_ */
