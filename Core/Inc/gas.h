/*
 * gas.h
 *
 *  Created on: 2026. 3. 18.
 *      Author: kimsuyeon
 *      ADC 읽기
 *      ppm 계산
 *      SAFE / WARNING / DANGER 상태 판정
 *      현재 상태 반환
 */

#ifndef INC_GAS_H_
#define INC_GAS_H_

#include "main.h"
#include <stdint.h>
#include "adc.h"

typedef enum {
    GAS_SAFE = 0,
    GAS_WARNING,
    GAS_DANGER
} GasLevel_t;

extern uint16_t adcValue[6];

uint16_t Gas_ReadADC(void);
uint16_t Gas_ReadADC_Avg(void);
uint32_t Gas_ADCtoVoltage_mV(uint16_t adc);
uint32_t Gas_ComputeRs(uint32_t vout_mV);
uint32_t Gas_ComputePPM(uint32_t rs);
GasLevel_t Gas_GetLevelFromPPM(uint32_t ppm);
GasLevel_t Gas_TaskPPM(void);

#endif /* INC_GAS_H_ */
