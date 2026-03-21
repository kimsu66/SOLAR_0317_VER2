/*
 * bms_safty_manager.c
 *
 *  Created on: 2026. 3. 21.
 *      Author: kimsuyeon
 */
#include "bms_safety_manager.h"
#include <string.h>

static uint8_t last_danger_count = 0;    // 현재 danger 개수
static uint8_t last_reduction_step = 0;  // 현재 감속량
static uint8_t danger_latched = 0;
static char danger_reason[64] = "SAFE";

void BMS_SAFETY_Reset(void)
{
    last_danger_count = 0;
    last_reduction_step = 0;
    danger_latched = 0;
    strcpy(danger_reason, "SAFE");
}

uint8_t GetReductionStep(void)
{
    return last_reduction_step;
}

uint8_t IsDangerLatched(void)
{
    return danger_latched;
}

uint8_t GetWarningCount(void)
{
    uint8_t count = 0;

    /* TEMP */
    if (GetTempState() == TEMP_STATE_WARNING)
        count++;

    /* GAS */
    if (GetGasState() == GAS_WARNING)
        count++;

    /* BMS: current or voltage 중 하나라도 warning이면 +1 */
    if (GetCurrentState() == CURRENT_STATE_WARNING ||
        GetVoltageState() == VOLTAGE_STATE_UNDER_WARNING ||
        GetVoltageState() == VOLTAGE_STATE_OVER_WARNING)
        count++;

    return count;
}

uint8_t HasDangerState(void)
{
    if (GetTempState() == TEMP_STATE_DANGER)
        return 1;

    if (GetGasState() == GAS_DANGER)
        return 1;

    if (GetCurrentState() == CURRENT_STATE_DANGER)
        return 1;

    if (GetVoltageState() == VOLTAGE_STATE_UNDER_DANGER)
        return 1;

    if (GetVoltageState() == VOLTAGE_STATE_OVER_DANGER)
        return 1;

    return 0;
}

const char* GetDangerReason(void)
{
    return danger_reason;
}

static void UpdateDangerReasonNow(void)
{
    danger_reason[0] = '\0';

    if (GetTempState() == TEMP_STATE_DANGER)
        strcat(danger_reason, "TEMP ");

    if (GetGasState() == GAS_DANGER)
        strcat(danger_reason, "GAS ");

    if (GetCurrentState() == CURRENT_STATE_DANGER)
        strcat(danger_reason, "CUR ");

    if (GetVoltageState() == VOLTAGE_STATE_OVER_DANGER)
        strcat(danger_reason, "V_OVER ");

    if (GetVoltageState() == VOLTAGE_STATE_UNDER_DANGER)
        strcat(danger_reason, "V_UNDER ");

    if (danger_reason[0] == '\0')
        strcpy(danger_reason, "UNKNOWN");
}

void BMS_SAFETY_UpdateSpeed(uint8_t actual_speed, int8_t *current_speed)
{
    static uint32_t speed_time = 0;
    static uint32_t safe_time = 0;
    static uint32_t recover_time = 0;

    uint8_t warning_count;
    uint8_t reduction_step = 0;
    uint32_t now = HAL_GetTick();

    warning_count = GetWarningCount();
    last_danger_count = warning_count;

    if (warning_count == 1)      reduction_step = 1;
    else if (warning_count == 2) reduction_step = 3;
    else if (warning_count >= 3) reduction_step = 5;

    last_reduction_step = reduction_step;

    if (actual_speed == 0)
    {
        speed_time = now;
        safe_time = now;
        return;
    }

    if (HasDangerState())
    {
				if (!danger_latched)
				{
						UpdateDangerReasonNow();
				}

        danger_latched = 1;
    }

    if (danger_latched)
    {
        if (now - speed_time >= 1000)
        {
            speed_time = now;

            if (*current_speed > 10)
                *current_speed -= 10;
            else
                *current_speed = 0;
        }
        return;
    }

    if (warning_count > 0)
    {
        safe_time = now;
        recover_time = now;

        if (now - speed_time >= 1000)
        {
            speed_time = now;

            if (*current_speed > 30 + reduction_step)
                *current_speed -= reduction_step;
            else
                *current_speed = 30;
        }
    }
    else
    {
        if (safe_time == 0)
            safe_time = now;

        if (now - safe_time >= 3000)
        {
						if (now - recover_time >= 200)   // 200ms마다 +1
						{
								recover_time = now;

								if (*current_speed < 80)
										(*current_speed)++;
						}
        }

        speed_time = now;
    }
}
