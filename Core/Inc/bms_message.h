/*
 * bms_message.h
 *
 *  Created on: 2026. 3. 20.
 *      Author: kimsuyeon
 */

#ifndef INC_BMS_MESSAGE_H_
#define INC_BMS_MESSAGE_H_

#include <bms_gas.h>
#include <bms_ina219.h>
#include <bms_temp.h>
#include "main.h"
#include "statemachine.h"
#include "bms_sensor.h"
#include "bms_safety_manager.h"

void SHOW_UART2_BMS(void);
void SHOW_UART6_BMS(void);


//const char* STMACHINE_GetDangerReason(void);

#endif /* INC_BMS_MESSAGE_H_ */
