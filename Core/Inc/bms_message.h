/*
 * bms_message.h
 *
 *  Created on: 2026. 3. 20.
 *      Author: kimsuyeon
 */

#ifndef INC_BMS_MESSAGE_H_
#define INC_BMS_MESSAGE_H_

#include "main.h"
#include "temp.h"
#include "gas2.h"
#include "ina219_bms.h"
#include "statemachine.h"

void SHOW_UART2_BMS(void);
void SHOW_UART6_BMS(void);


//const char* STMACHINE_GetDangerReason(void);

#endif /* INC_BMS_MESSAGE_H_ */
