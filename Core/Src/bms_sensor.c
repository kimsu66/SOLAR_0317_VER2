/*
 * bms_sensor.c
 *
 *  Created on: 2026. 3. 21.
 *      Author: kimsuyeon
 *
 *			TEMP task, GAS task, INA219(BMS 전압/전류) task
 *			각 상태값, 측정값 저장
 *			getter 제공
 */

#include "bms_sensor.h"


/* ===== TEMP 상태 관리 ===== */
static TEMP_STATE temp_state = TEMP_STATE_SAFE;
static uint16_t temp_adc = 0;
static int16_t temp_c = 0;

static uint32_t temp_prev_tick = 0;
static const uint32_t tempTaskTick = 100; // /* temp state update period */

/* ===== GAS 상태 관리 ===== */
static GasLevel_t gas_state = GAS_SAFE;
static uint16_t gas_adc = 0;

static uint32_t gas_prev_tick = 0;
static const uint32_t gasTaskTick = 100; // /* gas state update period */

/* ===== VOLTAGE, CURRENT 상태 관리 ===== */
static VOLTAGE_STATE bms_voltage_state = VOLTAGE_STATE_SAFE;
static CURRENT_STATE bms_current_state = CURRENT_STATE_SAFE;

static int32_t bms_voltage_mV = 0;
static int32_t bms_current_mA = 0;

static uint32_t bms_prev_tick = 0;
static const uint32_t bmsTaskTick = 100; // /* bms state update period */


/* ================= INIT ================= */

void BMS_SENSOR_Init(void)
{
    INA219_BMS_Init();
}

/* ================= TEMP TASK ================= */

static void TEMP_TASK(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - temp_prev_tick) < tempTaskTick) return;
    temp_prev_tick = now;

    temp_adc = Temp_ReadADC();
    temp_c   = Temp_CalcCelsius(temp_adc);
    temp_state = Temp_GetState(temp_c);
}

uint16_t GetTempADC(void)
{
    return temp_adc;
}

int16_t GetTempC(void)
{
    return temp_c;
}

TEMP_STATE GetTempState(void)
{
    return temp_state;
}

/* ================= GAS TASK ================= */

static void GAS_TASK(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - gas_prev_tick) < gasTaskTick) return;
    gas_prev_tick = now;

    gas_adc = Gas_ReadADC();
    gas_state = Gas_GetLevelFromADC(gas_adc);
}

uint16_t GetGasADC(void)
{
    return gas_adc;
}

GasLevel_t GetGasState(void)
{
    return gas_state;
}


/* ================= VOLTAGE, CURRENT TASK ================= */

static void INA219_BMS_TASK(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - bms_prev_tick) < bmsTaskTick) return;
    bms_prev_tick = now;

    bms_voltage_mV = INA219_BMS_ReadVoltage_mV();
    bms_current_mA = INA219_BMS_ReadCurrent_mA();

    bms_voltage_state = INA219_BMS_GetVoltageState(bms_voltage_mV);
    bms_current_state = INA219_BMS_GetCurrentState(bms_current_mA);
}

int32_t GetVoltagemV(void)
{
    return bms_voltage_mV;
}

VOLTAGE_STATE GetVoltageState(void)
{
    return bms_voltage_state;
}

int32_t GetCurrentmA(void)
{
    return bms_current_mA;
}

CURRENT_STATE GetCurrentState(void)
{
    return bms_current_state;
}


/* ================= PUBLIC TASK ================= */

void BMS_SENSOR_Task(void)
{
    TEMP_TASK();
    GAS_TASK();
    INA219_BMS_TASK();
}


