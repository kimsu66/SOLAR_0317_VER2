/*
 * temp.h
 *
 *  Created on: 2026. 3. 18.
 *      Author: kimsuyeon
 *
 *      [Main role]
 *
 *      ADC 읽기
 *      온도 계산
 *      SAFE / WARNING / DANGER 상태 판정
 *      현재 상태 반환
 */

#ifndef INC_TEMP_H_
#define INC_TEMP_H_

#include "main.h"
#include <stdint.h>
#include "adc.h"

typedef enum {
    TEMP_STATE_SAFE = 0,
    TEMP_STATE_WARNING,
    TEMP_STATE_DANGER
} TEMP_STATE;

extern uint16_t adcValue[6];

uint16_t Temp_ReadADC(void);
int16_t Temp_CalcCelsius(uint16_t adc);
TEMP_STATE Temp_GetState(int16_t temp_c);

/* 주기적으로 호출해서 현재 temp state만 얻는 함수 */
TEMP_STATE Temp_Task(void);

#endif /* INC_TEMP_H_ */
