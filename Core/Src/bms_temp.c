/*
 * temp.c
 *
 *  Created on: 2026. 3. 18.
 *      Author: kimsuyeon
 *
 */

#include <bms_temp.h>


/* threshold */
#define TEMP_WARNING_THRESHOLD   50
#define TEMP_DANGER_THRESHOLD    70

#define ADC_MAX     4095
#define VREF_mV     3300

#define R_FIXED     10000
#define R0          10000
#define BETA        3434
#define T0          29815   // 25C = 298.15K * 100

/* 내부 현재 상태 저장 */
static TEMP_STATE s_temp_state = TEMP_STATE_SAFE;

uint16_t Temp_ReadADC(void)
{
    return adcValue[0];
}

/* 간단 ln 근사 */
static int32_t ln_approx(int32_t x)
{
    return (2000 * (x - 1000)) / (x + 1000);
}

int16_t Temp_CalcCelsius(uint16_t adc)
{
    int32_t vout;
    int32_t r_ntc;
    int32_t ratio;
    int32_t ln_val;
    int32_t tempK;
    int16_t tempC;

    vout = (adc * VREF_mV) / ADC_MAX;

    if (vout <= 0 || vout >= VREF_mV) return 0;

    r_ntc = (R_FIXED * vout) / (VREF_mV - vout);
    ratio = (r_ntc * 1000) / R0;
    ln_val = ln_approx(ratio);

    tempK = (T0 * BETA) / (BETA + ln_val);
    tempC = (tempK - 27315) / 100;

    return tempC;
}

TEMP_STATE Temp_GetState(int16_t temp_c)
{
    if (temp_c >= TEMP_DANGER_THRESHOLD) {
        return TEMP_STATE_DANGER;
    }
    else if (temp_c >= TEMP_WARNING_THRESHOLD) {
        return TEMP_STATE_WARNING;
    }
    else {
        return TEMP_STATE_SAFE;
    }
}

TEMP_STATE Temp_Task(void)
{
    uint16_t adc;
    int16_t temp_c;

    adc = Temp_ReadADC();
    temp_c = Temp_CalcCelsius(adc);
    s_temp_state = Temp_GetState(temp_c);

    return s_temp_state;
}
