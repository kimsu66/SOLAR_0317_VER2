/*
 * bms_safty_manager.h
 *
 *  Created on: 2026. 3. 21.
 *      Author: kimsuyeon
 */

#ifndef INC_BMS_SAFETY_MANAGER_H_
#define INC_BMS_SAFETY_MANAGER_H_

#include <stdint.h>
#include "bms_sensor.h"

//static void UpdateCurrentSpeed(void);
/* statemachine에서 안전 상태 갱신용 */
void BMS_SAFETY_UpdateSpeed(uint8_t actual_speed, int8_t *current_speed);

// for BMS message
uint8_t GetWarningCount(void);
uint8_t GetReductionStep(void);
uint8_t IsDangerLatched(void);
const char* GetDangerReason(void);
void BMS_SAFETY_Reset(void);
uint8_t HasDangerState(void);

#endif /* INC_BMS_SAFETY_MANAGER_H_ */
