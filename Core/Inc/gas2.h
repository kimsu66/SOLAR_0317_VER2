/*
 * gas2.h
 *
 *  Created on: 2026. 3. 20.
 *      Author: kimsuyeon
 */

#ifndef INC_GAS2_H_
#define INC_GAS2_H_


#include "main.h"

typedef enum
{
    GAS_SAFE = 0,
    GAS_WARNING,
    GAS_DANGER
} GasLevel_t;

extern uint16_t adcValue[6];

uint16_t Gas_ReadADC(void);
uint16_t Gas_ReadADC_Avg(void);

void Gas_BaselineInit(void);
void Gas_UpdateBaseline(void);
uint8_t Gas_IsBaselineReady(void);
uint16_t Gas_GetBaselineADC(void);

uint16_t Gas_ComputeDelta(uint16_t adc_now, uint16_t adc_base);
uint16_t Gas_ComputeScore(void);

GasLevel_t Gas_GetLevelFromScore(uint16_t score);


GasLevel_t Gas_GetLevelFromADC(uint16_t adc);
GasLevel_t Gas_TaskLevel(void);

#endif /* INC_GAS2_H_ */
