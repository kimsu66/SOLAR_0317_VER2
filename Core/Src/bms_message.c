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

//const char* STMACHINE_GetDangerReason(void)
//{
//    static char current_reason[64];
//    static char latched_reason[64] = "UNKNOWN";
//
//    current_reason[0] = '\0';
//
//    if (STMACHINE_GetTempState() == TEMP_STATE_DANGER)
//        strcat(current_reason, "TEMP ");
//
//    if (STMACHINE_GetGasState() == GAS_DANGER)
//        strcat(current_reason, "GAS ");
//
//    if (STMACHINE_GetBmsVoltageState() == VOLTAGE_STATE_DANGER)
//        strcat(current_reason, "VOLT ");
//
//    if (STMACHINE_GetBmsCurrentState() == CURRENT_STATE_DANGER)
//        strcat(current_reason, "CUR ");
//
//    /* 현재 danger 원인이 있으면 그걸 latched_reason으로 갱신 */
//    if (current_reason[0] != '\0')
//    {
//        strcpy(latched_reason, current_reason);
//        return latched_reason;
//    }
//
//    /* 현재는 SAFE여도, danger latch 상태면 마지막 danger 원인 유지 */
//    if (STMACHINE_IsDangerLatched())
//    {
//        return latched_reason;
//    }
//
//    return "SAFE";
//}

static const char* SPEED_CTRL_STR(void)
{
		static char ctrl_msg[64];

		if (STMACHINE_IsDangerLatched())
		{
				snprintf(ctrl_msg, sizeof(ctrl_msg),
								 "DANGER (%s) -> TO 0",
								 STMACHINE_GetDangerReason());
				return ctrl_msg;
		}

    if (STMACHINE_GetWarningCount() > 0)
    {
        static char ctrl_msg[32];
        snprintf(ctrl_msg, sizeof(ctrl_msg),
                 "WARNING x%d (-%d)",
                 STMACHINE_GetWarningCount(),
                 STMACHINE_GetReductionStep());
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
    	                       STMACHINE_GetModeString(),
    	                       STMACHINE_GetActualSpeed(),
    	                       STMACHINE_GetTraceString(),
    	                       TEMP_STATE_STR(STMACHINE_GetTempState()),
    	                       STMACHINE_GetTempC(),
    	                       GAS_STATE_STR(STMACHINE_GetGasState()),
														 Gas_ReadADC(),
//    	                       STMACHINE_GetGasScore(),
    	                       VOLTAGE_STATE_STR(STMACHINE_GetBmsVoltageState()),
    	                       STMACHINE_GetBmsVoltagemV(),
    	                       CURRENT_STATE_STR(STMACHINE_GetBmsCurrentState()),
    	                       STMACHINE_GetBmsCurrentmA(),
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
                               STMACHINE_GetModeString(),
                               STMACHINE_GetActualSpeed(),
                               STMACHINE_GetTraceString(),
                               TEMP_STATE_STR(STMACHINE_GetTempState()),
                               STMACHINE_GetTempC(),
                               GAS_STATE_STR(STMACHINE_GetGasState()),
															 Gas_ReadADC(),
//                               STMACHINE_GetGasScore(),
                               VOLTAGE_STATE_STR(STMACHINE_GetBmsVoltageState()),
                               STMACHINE_GetBmsVoltagemV(),
                               CURRENT_STATE_STR(STMACHINE_GetBmsCurrentState()),
                               STMACHINE_GetBmsCurrentmA(),
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
