/*
 * statemachine.h
 *
 *  Created on: Mar 1, 2026
 *      Author: appletea
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#include "car.h"
#include "ultrasonic.h"

// KSY
#include "adc.h"
#include "bms_safety_manager.h"

//LSH
#include "trace.h"



#define INIT_SPEED 	80

#define Block_Distance_Front 35
#define Block_Distance_Side 20
#define Crash_Distance 10
#define Stuck_Timeout 300

// ================= 기준 값 =================

#define BACK_DIST       10
#define LRTH            15
#define CRITICAL_DIST   40
#define TURN_THRESHOLD  20

typedef enum
{
	AUTO_STATE_SCAN,
	AUTO_STATE_STOP,
	AUTO_STATE_AVOID,
	AUTO_STATE_BACK,
}AUTO_STATE;

// 초기화: UART1 인터럽트 시작 등
void STMACHINE_Init(void);
void ST_FLAG(uint8_t cmd);
void ST_MACHINE();
void DC_CONTROL_AUTO();

/* ================= DEBUG ================= */
void SHOW_UART2_ULTRASONIC();
void SHOW_UART2_SPEED();
void SHOW_UART2_TRACE();

const char* GetModeString(void);
const char* GetTraceString(void);
uint8_t GetActualSpeed(void);


#endif /* INC_STATEMACHINE_H_ */
