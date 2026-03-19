/*
 * trace.c
 *
 *  Created on: 2026. 3. 16.
 *      Author: kimsuyeon
 */



#include "trace.h"

uint16_t sensor_buffer[6][FILTER_SIZE];
uint8_t filter_index = 0;
uint32_t sum;

uint16_t S1, S2, S3, S4;
int error_x, error_y;

//// ADC 폴링방법 예시(채널 부족시 사용할 예정)
//void Read_Sensors()
//{
//    uint16_t value[4];
//
//    for(int i=0;i<4;i++)
//    {
//        HAL_ADC_Start(&hadc1);
//        HAL_ADC_PollForConversion(&hadc1,10);
//        value[i] = HAL_ADC_GetValue(&hadc1);
//    }
//
//    S1 = value[0];
//    S2 = value[1];
//    S3 = value[2];
//    S4 = value[3];
//}

// 10개의 adc값을 모아서 개수만큼 나눈 평균값을 사용(노이즈 제거)
void Sensor_Filter()
{
  for(uint8_t i = 2; i < 6; i++) sensor_buffer[i][filter_index] = adcValue[i];
  filter_index++;
  if(filter_index >= FILTER_SIZE) filter_index = 0;

  sum = 0;
  for(uint8_t i = 0; i < FILTER_SIZE; i++) sum += sensor_buffer[2][i];
  S1 = sum / FILTER_SIZE;

  sum = 0;
  for(uint8_t i = 0; i < FILTER_SIZE; i++) sum += sensor_buffer[3][i];
  S2 = sum / FILTER_SIZE;

  sum = 0;
  for(uint8_t i = 0; i < FILTER_SIZE; i++) sum += sensor_buffer[4][i];
  S3 = sum / FILTER_SIZE;

  sum=0;
  for(uint8_t i = 0; i < FILTER_SIZE; i++) sum += sensor_buffer[5][i];
  S4 = sum / FILTER_SIZE;
}

// 태양 방향 계산
void Sun_Position()
{
  int left  = S1 + S3;
  int right = S2 + S4;

  int top    = S1 + S2;
  int bottom = S3 + S4;

  error_x = left - right;
  error_y = top - bottom;
}

int target_pan  = 70;
int target_tilt = 70;

// 목표 서보각 계산
void Target_Update()
{
  if(error_x > THRESHOLD) target_pan--;
  else if(error_x < -THRESHOLD) target_pan++;

//  // 수평면의 서보모터에서 180도 초과 회전이 불가능한 걸 구현하기 위한 테스트 코드
//  if(error_x > THRESHOLD)
//  {
//    if(target_pan >= 20) target_pan--;
//    else target_tilt++;
//  }
//
//  else if(error_x < -THRESHOLD)
//  {
//    if(target_pan <= 120) target_pan++;
//    else target_tilt--;
//  }

  if(error_y > THRESHOLD) target_tilt++;
  else if(error_y < -THRESHOLD) target_tilt--;

  // 최소, 최대각 설정(ARR 1000기준 CCR 20~120 동작)
  if(target_pan < 20) target_pan = 20;
  if(target_pan > 120) target_pan = 120;

  if(target_tilt < 40) target_tilt = 40;
  if(target_tilt > 110) target_tilt = 110;
}

int pan = 70;
int tilt = 70;

// 서보모터 이동 제어
void Servo_Move()
{
  if(pan < target_pan) pan++;
  else if(pan > target_pan) pan--;

  if(tilt < target_tilt) tilt++;
  else if(tilt > target_tilt) tilt--;

  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pan);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, tilt);
}
