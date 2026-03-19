/*
 * trace.h
 *
 *  Created on: 2026. 3. 16.
 *      Author: kimsuyeon
 */

#ifndef INC_TRACE_H_
#define INC_TRACE_H_

#include "stm32f4xx_hal.h"
#include "tim.h"

#define FILTER_SIZE 10
// 좌우, 상하 오차 관련 변수(800미만시 상단이 정지하지 않고 조금씩 움직임)
#define THRESHOLD 800

extern uint16_t adcValue[6];
extern uint16_t S1,S2,S3,S4;
extern int error_x;
extern int error_y;

void Sensor_Filter();
void Sun_Position();
void Target_Update();
void Servo_Move();



#endif /* INC_TRACE_H_ */
