/*
 * gas.c
 *
 *  Created on: 2026. 3. 6.
 *      Author: kimsuyeon
 */

#include "gas.h"
#include <stdio.h>

#define GAS_SAMPLE_COUNT        8
#define ADC_MAX_VALUE           4095
#define VCC_MV                  3300

/* MQ135 회로 */
#define RL_VALUE                20000   // 20kΩ

/* calibration */
#define RO_CLEAN_AIR_FACTOR     36      // 3.6 * 10

/* ppm threshold */
#define GAS_WARNING_PPM         100
#define GAS_DANGER_PPM          400

static uint32_t Ro = 10000;   // 초기값


uint16_t Gas_ReadADC(void)
{
    return adcValue[1];
}

uint16_t Gas_ReadADC_Avg(void)
{
    uint32_t sum = 0;

    for (int i = 0; i < GAS_SAMPLE_COUNT; i++)
    {
        sum += Gas_ReadADC();
    }

    return (uint16_t)(sum / GAS_SAMPLE_COUNT);
}

/* ADC → mV */
uint32_t Gas_ADCtoVoltage_mV(uint16_t adc)
{
    return ((uint32_t)adc * VCC_MV) / ADC_MAX_VALUE;
}

/* Voltage → Rs */
uint32_t Gas_ComputeRs(uint32_t vout_mV)
{
    if (vout_mV == 0)
        return 0;

    return (RL_VALUE * (VCC_MV - vout_mV)) / vout_mV;
}

/*
 * 근사 ppm 계산
 * 실제 MQ135 그래프는 로그지만 임베디드 프로젝트에서는 보통 ratio 기반으로 근사
 */
uint32_t Gas_ComputePPM(uint32_t rs)
{
    uint32_t ratio = (rs * 100) / Ro;

    if (ratio > 200)
        return 50;

    if (ratio > 150)
        return 100; // warning

    if (ratio > 120)
        return 200;

    if (ratio > 100)
        return 400; // danger

    if (ratio > 80)
        return 800;

    return 1200;
}

GasLevel_t Gas_GetLevelFromPPM(uint32_t ppm)
{
    if (ppm >= GAS_DANGER_PPM)  return GAS_DANGER;
    if (ppm >= GAS_WARNING_PPM) return GAS_WARNING;
    return GAS_SAFE;
}

GasLevel_t Gas_TaskPPM(void)
{
    uint16_t adc = Gas_ReadADC_Avg();
    uint32_t vout = Gas_ADCtoVoltage_mV(adc);
    uint32_t rs = Gas_ComputeRs(vout);
    uint32_t ppm = Gas_ComputePPM(rs);

    return Gas_GetLevelFromPPM(ppm);
}
