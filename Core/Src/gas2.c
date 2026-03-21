/*
 * gas2.c
 *
 *  Created on: 2026. 3. 20.
 *      Author: kimsuyeon
 */


#include "gas2.h"
#include <stdio.h>

#define GAS_SAMPLE_COUNT            8
#define GAS_BASELINE_SAMPLE_COUNT   100

/* baseline 대비 변화량 기준 */
#define GAS_WARNING_DELTA           150
#define GAS_DANGER_DELTA            400

#define GAS_WARNING_ADC   1500
#define GAS_DANGER_ADC    2500

static uint16_t gas_baseline_adc = 0;
static uint8_t gas_baseline_ready = 0;

/* ================= ADC READ ================= */

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

/* ================= BASELINE ================= */

void Gas_BaselineInit(void)
{
    gas_baseline_adc = 0;
    gas_baseline_ready = 0;
}

/*
 * 시작 직후 안정된 공기 상태에서 baseline 형성
 * 여러 번 평균내서 baseline 설정
 */
void Gas_UpdateBaseline(void)
{
    static uint32_t baseline_sum = 0;
    static uint16_t baseline_count = 0;

    if (gas_baseline_ready) return;

    baseline_sum += Gas_ReadADC_Avg();
    baseline_count++;

    if (baseline_count >= GAS_BASELINE_SAMPLE_COUNT)
    {
        gas_baseline_adc = (uint16_t)(baseline_sum / baseline_count);
        gas_baseline_ready = 1;
    }
}

uint8_t Gas_IsBaselineReady(void)
{
    return gas_baseline_ready;
}

uint16_t Gas_GetBaselineADC(void)
{
    return gas_baseline_adc;
}

/* ================= GAS SCORE ================= */

/*
 * baseline 대비 얼마나 변했는지 계산
 * MQ135 모듈은 환경/회로에 따라 증가/감소 방향이 달라질 수 있어서
 * 절대 차이값으로 처리
 */
uint16_t Gas_ComputeDelta(uint16_t adc_now, uint16_t adc_base)
{
    if (adc_now >= adc_base)
        return (adc_now - adc_base);
    else
        return (adc_base - adc_now);
}

/*
 * 사람이 보기 쉬운 상대 지표
 * baseline 대비 변화량을 그대로 score처럼 사용
 */
uint16_t Gas_ComputeScore(void)
{
    uint16_t adc_now = Gas_ReadADC_Avg();

    if (!gas_baseline_ready)
        return 0;

    return Gas_ComputeDelta(adc_now, gas_baseline_adc);
}

GasLevel_t Gas_GetLevelFromScore(uint16_t score)
{
    if (score >= GAS_DANGER_DELTA)  return GAS_DANGER;
    if (score >= GAS_WARNING_DELTA) return GAS_WARNING;
    return GAS_SAFE;
}

GasLevel_t Gas_GetLevelFromADC(uint16_t adc)
{
    if (adc >= GAS_DANGER_ADC)  return GAS_DANGER;
    if (adc >= GAS_WARNING_ADC) return GAS_WARNING;
    return GAS_SAFE;
}

GasLevel_t Gas_TaskLevel(void)
{
	uint16_t adc = Gas_ReadADC_Avg();
	return Gas_GetLevelFromADC(adc);
}
