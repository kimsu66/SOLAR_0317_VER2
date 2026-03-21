/*
 * bms_message.c
 *
 *  Created on: 2026. 3. 20.
 *      Author: kimsuyeon
 */


#include "bms_message.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

/* UART6 DMA TX용 */
static char uart6_msg[128];
static volatile uint8_t uart6_tx_busy = 0;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart6)
    {
        uart6_tx_busy = 0;
    }
}

static const char* TEMP_STATE_STR(TEMP_STATE state)
{
    switch (state)
    {
        case TEMP_STATE_SAFE:    return "[SAFE]";
        case TEMP_STATE_WARNING: return "[WARNING]";
        case TEMP_STATE_DANGER:  return "[DANGER]";
        default:                 return "[UNKNOWN]";
    }
}

static const char* GAS_STATE_STR(GasLevel_t state)
{
    switch (state)
    {
        case GAS_SAFE:    return "[SAFE]";
        case GAS_WARNING: return "[WARNING]";
        case GAS_DANGER:  return "[DANGER]";
        default:          return "[UNKNOWN]";
    }
}

static const char* CURRENT_STATE_STR(CURRENT_STATE state)
{
    switch (state)
    {
        case CURRENT_STATE_SAFE:    return "[SAFE]";
        case CURRENT_STATE_WARNING: return "[WARNING]";
        case CURRENT_STATE_DANGER:  return "[DANGER]";
        default:                return "[UNKNOWN]";
    }
}

static const char* VOLTAGE_STATE_STR(VOLTAGE_STATE state)
{
    switch (state)
    {
        case VOLTAGE_STATE_SAFE:    return "[SAFE]";
        case VOLTAGE_STATE_UNDER_WARNING: return "[WARNING]";
        case VOLTAGE_STATE_UNDER_DANGER: return "[DANGER]";
        case VOLTAGE_STATE_OVER_WARNING: return "[WARNING]";
        case VOLTAGE_STATE_OVER_DANGER:  return "[DANGER]";
        default:                    return "[UNKNOWN]";
    }
}

static const char* SPEED_CTRL_STR(void)
{
		static char ctrl_msg[64];

		if (IsDangerLatched())
		{
				snprintf(ctrl_msg, sizeof(ctrl_msg),
								 "DANGER (%s) -> TO 0",
								 GetDangerReason());
				return ctrl_msg;
		}

    if (GetWarningCount() > 0)
    {
        snprintf(ctrl_msg, sizeof(ctrl_msg),
                 "WARNING x%d (-%d)",
                 GetWarningCount(),
                 GetReductionStep());
        return ctrl_msg;
    }

    return "SAFE";
}

static void SEND_BMS_STATUS(UART_HandleTypeDef *huart, uint32_t *prev_time)
{
    uint32_t now = HAL_GetTick();
    int len;

    if ((now - *prev_time) < 1000) return;

    /* UART6 DMA 전송 중이면 건너뜀 */
    if (huart == &huart6 && uart6_tx_busy) return;

    *prev_time = now;

    if (huart == &huart6)
    {
    	len = snprintf(uart6_msg, sizeof(uart6_msg),
    	                       "<%s>\r\n"
    	                       "SPEED:%d "
    	                       "TRACE:%s\r\n"
    	                       "TEMP:%s %dC "
    	                       "GAS:%s %u "
    	                       "VOLT:%s %ldmV "
    	                       "CUR:%s %ldmA\r\n"
    	                       "<%s>\r\n",
    	                       GetModeString(),
    	                       GetActualSpeed(),
    	                       GetTraceString(),
    	                       TEMP_STATE_STR(GetTempState()),
    	                       GetTempC(),
    	                       GAS_STATE_STR(GetGasState()),
														 Gas_ReadADC(),
    	                       VOLTAGE_STATE_STR(GetVoltageState()),
    	                       GetVoltagemV(),
    	                       CURRENT_STATE_STR(GetCurrentState()),
    	                       GetCurrentmA(),
    	                       SPEED_CTRL_STR());

        if (len > 0)
        {
            uart6_tx_busy = 1;
            if (HAL_UART_Transmit_DMA(huart, (uint8_t *)uart6_msg, len) != HAL_OK)
            {
                uart6_tx_busy = 0;
            }
        }
    }
    else
    {
        /* UART2는 기존 방식 유지 */
        char msg[128];

        len = snprintf(msg, sizeof(msg),
                               "===== STATUS =====\r\n"
                               "<%s>\r\n"
                               "SPD:%d\r\n"
                               "TRACE:%s\r\n"
                               "TEMP:%s %dC "
                               "GAS:%s %u "
                               "VOLT:%s %ldmV "
                               "CUR:%s %ldmA\r\n"
                               "<%s>\r\n",
                               GetModeString(),
                               GetActualSpeed(),
                               GetTraceString(),
                               TEMP_STATE_STR(GetTempState()),
                               GetTempC(),
                               GAS_STATE_STR(GetGasState()),
															 Gas_ReadADC(),
                               VOLTAGE_STATE_STR(GetVoltageState()),
                               GetVoltagemV(),
                               CURRENT_STATE_STR(GetCurrentState()),
                               GetCurrentmA(),
                               SPEED_CTRL_STR());


        HAL_UART_Transmit(huart, (uint8_t *)msg, strlen(msg), 1000);
    }
}

void SHOW_UART2_BMS(void)
{
    static uint32_t prev_time_uart2 = 0;
    SEND_BMS_STATUS(&huart2, &prev_time_uart2);
}

void SHOW_UART6_BMS(void)
{
    static uint32_t prev_time_uart6 = 0;
    SEND_BMS_STATUS(&huart6, &prev_time_uart6);
}
