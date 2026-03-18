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
    BMS_STATE_SAFE = 0,
    BMS_STATE_WARNING,
    BMS_STATE_DANGER
} BMS_STATE;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t addr;
    int16_t current_lsb_uA;
} INA219_BMS_t;


uint8_t INA219_BMS_Init(void);
int32_t INA219_BMS_ReadVoltage_mV(void);
int32_t INA219_BMS_ReadCurrent_mA(void);
BMS_STATE INA219_BMS_GetState(int32_t voltage_mV, int32_t current_mA);
BMS_STATE INA219_BMS_Task(void);

#endif /* INC_INA219_BMS_H_ */
