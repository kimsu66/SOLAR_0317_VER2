/*
 * gas2.c
 *
 *  Created on: 2026. 3. 20.
 *      Author: kimsuyeon
 */


#include <bms_gas.h>
#include <stdio.h>

#define GAS_WARNING_ADC   1500
#define GAS_DANGER_ADC    2500


/* ================= ADC READ ================= */

uint16_t Gas_ReadADC(void)
{
    return adcValue[1];
}

GasLevel_t Gas_GetLevelFromADC(uint16_t adc)
{
    if (adc >= GAS_DANGER_ADC)  return GAS_DANGER;
    if (adc >= GAS_WARNING_ADC) return GAS_WARNING;
    return GAS_SAFE;
}

GasLevel_t Gas_TaskLevel(void)
{
	uint16_t adc = Gas_ReadADC();
	return Gas_GetLevelFromADC(adc);
}
