/*
 * statemachine.c
 *
 *  Created on: Mar 1, 2026
 *      Author: appletea
 */

#include "statemachine.h"




extern UART_HandleTypeDef huart1;

static volatile uint8_t rxData[1];
static volatile uint8_t rxFlag = 0;
static uint8_t rxCmd = 0;

static AUTO_STATE auto_st = AUTO_STATE_STOP;
static uint32_t auto_tick = 0;

// ADC value
uint16_t adcValue[6];

/* ===== 온도 상태 관리 ===== */
static TEMP_STATE temp_state = TEMP_STATE_SAFE;
static uint16_t temp_adc = 0;
static int16_t temp_c = 0;

static uint32_t temp_prev_tick = 0;

static const uint32_t tempTaskTick = 100; // /* temp state update period */

static const uint32_t tempUartTick = 500; // /* temp uart debug print period */
static uint32_t temp_uart_prevTick = 0;

/* ===== 가스 상태 관리 ===== */
static GasLevel_t gas_state = GAS_SAFE;
static uint16_t gas_adc = 0;
static uint32_t gas_ppm = 0;

static uint32_t gas_prev_tick = 0;

static const uint32_t gasTaskTick = 100; // /* gas state update period */

static const uint32_t gasUartTick = 500; // /* gas uart debug print period */
static uint32_t gas_uart_prevTick = 0;

/* ===== BMS 상태 관리 ===== */
static BMS_STATE bms_state = BMS_STATE_SAFE;
static int32_t bms_voltage_mV = 0;
static int32_t bms_current_mA = 0;

static uint32_t bms_prev_tick = 0;

static const uint32_t bmsTaskTick = 100; // /* bms state update period */

static const uint32_t bmsUartTick = 500; // /* bms uart debug print period */
static uint32_t bms_uart_prevTick = 0;

int8_t current_speed = 80;

static uint8_t actual_speed = 0;         // 실제 출력중인 속도
static uint8_t last_danger_count = 0;    // 현재 danger 개수
static uint8_t last_reduction_step = 0;  // 현재 감속량



/* ================= INIT ================= */

void STMACHINE_Init(void)
{
    // UART1 수신 인터럽트 시작
    HAL_UART_Receive_IT(&huart1, (uint8_t *)rxData, 1);

    // 초음파 함수 초기화
    Ultrasonic_Init();

    // 1) speed 모듈에 TIM2 채널 연결
    Speed_Init(&htim2, TIM_CHANNEL_1, &htim2, TIM_CHANNEL_2);

    // 2) PWM Start + 초기 STOP
    Car_Init();

    // 3) ADC INIT
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcValue, 6);

    // 4) INA219 BMS INIT
    INA219_BMS_Init();

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

/* ================= UART RX CALLBACK ================= */

// UART 수신 콜백 함수를 여기로 이동
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        rxCmd = rxData[0];
        rxFlag = 1;
        // 다음 수신 대기
        HAL_UART_Receive_IT(&huart1, (uint8_t *)rxData, 1);
    }
}

// 디버깅 화면에서 관찰할 변수
uint8_t debug_current_spd = 0;

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

/* =========================
 * Temp getter
 * - 다른 모듈/나중 로직에서 필요하면 사용
 * ========================= */
TEMP_STATE STMACHINE_GetTempState(void)
{
    return temp_state;
}

/* ================= GAS TASK ================= */

static void GAS_TASK(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - gas_prev_tick) < gasTaskTick) return;
    gas_prev_tick = now;

    gas_adc = Gas_ReadADC_Avg();
    gas_ppm = Gas_ComputePPM(Gas_ComputeRs(Gas_ADCtoVoltage_mV(gas_adc)));
    gas_state = Gas_GetLevelFromPPM(gas_ppm);
}

/* =========================
 * Gas getter
 * - 다른 모듈/나중 로직에서 필요하면 사용
 * ========================= */
GasLevel_t STMACHINE_GetGasState(void)
{
    return gas_state;
}

/* ================= BMS TASK ================= */

static void INA219_BMS_TASK(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - bms_prev_tick) < bmsTaskTick) return;
    bms_prev_tick = now;

    bms_voltage_mV = INA219_BMS_ReadVoltage_mV();
    bms_current_mA = INA219_BMS_ReadCurrent_mA();
    bms_state = INA219_BMS_GetState(bms_voltage_mV, bms_current_mA);
}

/* =========================
 * BMS getter
 * - 다른 모듈/나중 로직에서 필요하면 사용
 * ========================= */
BMS_STATE STMACHINE_GetBmsState(void)
{
    return bms_state;
}


///*** MANUAL 모드 로직 ***///
static void DC_CONTROL_MANUAL(uint8_t cmd)
{
    switch(cmd)
    {
        case 'F':
        	debug_current_spd = 100;
        	Car_Move(CAR_FRONT, SPD_100);
            break;
        case 'Q':
        	debug_current_spd = 50;
			Car_Move(CAR_FRONT, SPD_50);
			break;
        case 'B':
        	debug_current_spd = 100;
        	Car_Move(CAR_BACK, SPD_100);
            break;
        case 'W':
        	debug_current_spd = 50;
			Car_Move(CAR_BACK, SPD_50);
			break;
        case 'L':
        	debug_current_spd = 100;
        	Car_Move(CAR_LEFT, SPD_100);
            break;
        case 'E':
        	debug_current_spd = 50;
			Car_Move(CAR_LEFT, SPD_50);
			break;
        case 'R':
        	debug_current_spd = 100;
        	Car_Move(CAR_RIGHT, SPD_100);
            break;
        case 'T':
        	debug_current_spd = 50;
			Car_Move(CAR_RIGHT, SPD_50);
			break;
        case 'S':
        	Car_Stop();
        	break;
        default:
            break;
    }
}

static void DC_CONTROL_MANUAL_PERCENT(uint8_t cmd)
{
    switch(cmd)
    {
        case 'F':
        	Car_MovePercent(CAR_FRONT, current_speed);
        	actual_speed = current_speed;
            break;
        case 'Q':
        	Car_MovePercent(CAR_FRONT, current_speed);
        	actual_speed = current_speed;
        	break;
        case 'B':
        	Car_MovePercent(CAR_BACK, current_speed);
        	actual_speed = current_speed;
            break;
        case 'W':
        	Car_MovePercent(CAR_BACK, current_speed);
        	actual_speed = current_speed;
        	break;
        case 'L':
        	Car_MovePercent(CAR_LEFT, current_speed);
        	actual_speed = current_speed;
           break;
        case 'E':
        	Car_MovePercent(CAR_LEFT, current_speed);
        	actual_speed = current_speed;
        	break;
        case 'R':
        	Car_MovePercent(CAR_RIGHT, current_speed);
        	actual_speed = current_speed;
            break;
        case 'T':
        	Car_MovePercent(CAR_RIGHT, current_speed);
        	actual_speed = current_speed;
        	break;
        case 'S':
        	Car_Stop();
        	actual_speed = 0;
        	break;
        default:
        	Car_Stop();
        	actual_speed = 0;
            break;
    }
}

///*** UART2(테스트용) ***///


/* ================= ULTRASONIC DEBUG ================= */

const static uint32_t waitTick = 200;
static uint32_t prevTick = 0;

void SHOW_UART2()
{
	Ultrasonic_TriggerAll();
	uint32_t currentTick = HAL_GetTick();
		if ((currentTick - prevTick) < waitTick) return; // 200ms 아직 안 됨
		prevTick = currentTick;
	printf("LEFT : %d cm\r\n CENTER : %d cm\r\n RIGHT : %d cm\r\n",
			Ultrasonic_GetDistanceCm(US_LEFT),
			Ultrasonic_GetDistanceCm(US_CENTER),
			Ultrasonic_GetDistanceCm(US_RIGHT));
}


/* ================= TEMP DEBUG ================= */

void SHOW_UART2_TEMP(void)
{
    uint32_t now = HAL_GetTick();
    if ((now - temp_uart_prevTick) < tempUartTick) return;
    temp_uart_prevTick = now;

    printf("[TEMP] ADC: %u | TEMP: %d C | STATE: ", temp_adc, temp_c);

    switch (temp_state)
    {
        case TEMP_STATE_SAFE:
            printf("SAFE\r\n");
            break;

        case TEMP_STATE_WARNING:
            printf("WARNING\r\n");
            break;

        case TEMP_STATE_DANGER:
            printf("DANGER\r\n");
            break;

        default:
            printf("UNKNOWN\r\n");
            break;
    }
}

/* ================= GAS DEBUG ================= */

void SHOW_UART2_GAS(void)
{
    uint32_t now = HAL_GetTick();
    if ((now - gas_uart_prevTick) < gasUartTick) return;
    gas_uart_prevTick = now;

    printf("[GAS] ADC: %u | PPM: %lu | STATE: ", gas_adc, gas_ppm);

    switch (gas_state)
    {
        case GAS_SAFE:
            printf("SAFE\r\n");
            break;

        case GAS_WARNING:
            printf("WARNING\r\n");
            break;

        case GAS_DANGER:
            printf("DANGER\r\n");
            break;

        default:
            printf("UNKNOWN\r\n");
            break;
    }
}

/* ================= BMS DEBUG ================= */

void SHOW_UART2_BMSCurrent(void)
{
    uint32_t now = HAL_GetTick();
    if ((now - bms_uart_prevTick) < bmsUartTick) return;
    bms_uart_prevTick = now;

    printf("[BMS] V: %ld mV | I: %ld mA | STATE: ",
           bms_voltage_mV, bms_current_mA);

    switch (bms_state)
    {
        case BMS_STATE_SAFE:
            printf("SAFE\r\n");
            break;

        case BMS_STATE_WARNING:
            printf("WARNING\r\n");
            break;

        case BMS_STATE_DANGER:
            printf("DANGER\r\n");
            break;

        default:
            printf("UNKNOWN\r\n");
            break;
    }
}

void SHOW_UART2_SPEED(void)
{
    static uint32_t speed_uart_prevTick = 0;
    uint32_t now = HAL_GetTick();

    if ((now - speed_uart_prevTick) < 500) return;
    speed_uart_prevTick = now;

    printf("[SPEED] current_speed: %d\r\n", current_speed);
}

void SHOW_UART2_BMS(void)
{
    static uint32_t prev_time = 0;
    uint32_t now = HAL_GetTick();

    if ((now - prev_time) < 1000) return;
    prev_time = now;

    if (actual_speed == 0)
		{
				printf("[STOP] speed:0");

				if (temp_state == TEMP_STATE_DANGER)
				{
						printf(" | TEMP:%dC", temp_c);
				}

				if (gas_state == GAS_DANGER)
				{
						printf(" | GAS:%luppm", gas_ppm);
				}

				if (bms_state == BMS_STATE_DANGER)
				{
						printf(" | CURRENT:%ldmA", bms_current_mA);
				}

				printf("\r\n");
				return;
		}

    if (last_danger_count == 0)
    {
        printf("[SAFE] speed:%d\r\n", actual_speed);
    }
    else
    {
        printf("[DANGER] speed:%d reduction:-%d | ",
               actual_speed,
               last_reduction_step);

        if (temp_state == TEMP_STATE_DANGER)
        {
            printf("TEMP:%dC ", temp_c);
        }

        if (gas_state == GAS_DANGER)
        {
            printf("GAS:%luppm ", gas_ppm);
        }

        if (bms_state == BMS_STATE_DANGER)
        {
            printf("CURRENT:%ldmA ", bms_current_mA);
        }

        printf("\r\n");
    }
}

void SHOW_UART2_TRACE(void)
{
    static uint32_t trace_uart_prevTick = 0;
    uint32_t now = HAL_GetTick();

    if ((now - trace_uart_prevTick) < 500) return;
    trace_uart_prevTick = now;

    if (actual_speed != 0)
    {
        printf("[TRACE] RUNNING OFF | CAR MOVING\r\n");
        return;
    }

    printf("[TRACE] ADC : %d | %d | %d | %d\r\n", S1, S2, S3, S4);
    printf("[TRACE] X : %d | Y : %d\r\n", error_x, error_y);
}



/* ================= AUTO ================= */

void DC_CONTROL_AUTO() {
    Ultrasonic_TriggerAll();
    uint32_t current_Tick = HAL_GetTick();

    uint8_t L = Ultrasonic_GetDistanceCm(US_LEFT);
    uint8_t C = Ultrasonic_GetDistanceCm(US_CENTER);
    uint8_t R = Ultrasonic_GetDistanceCm(US_RIGHT);

    // 100 이상의 거리는 100으로 통일
    uint8_t DisLeft   = (L == 0 || L > 100) ? 100 : L;
    uint8_t DisCenter = (C == 0 || C > 100) ? 100 : C;
    uint8_t DisRight  = (R == 0 || R > 100) ? 100 : R;

    //3개 반사값 비교해서 우선판단권 넘김
    uint8_t front_min = DisCenter;
    if (DisLeft < front_min)  front_min = DisLeft;
    if (DisRight < front_min) front_min = DisRight;

    switch (auto_st) {
        case AUTO_STATE_SCAN:
            // 최우선 순위 정면 충돌 직전 시 500ms 후진
            if (DisCenter < Crash_Distance) {
                Car_Move(CAR_BACK, SPD_50);
                auto_tick = current_Tick;
                auto_st = AUTO_STATE_BACK;
            }
            // 본격적인 회피
            else if (front_min < Block_Distance_Front) {
                // 이중 비교 및 양 옆센서 값이 너무 낮을때 회피
            	if (DisLeft < (Block_Distance_Side)) {
                    Car_Move(CAR_RIGHT, SPD_80);
                }
                else if (DisRight < (Block_Distance_Side)) {
                    Car_Move(CAR_LEFT, SPD_80);
                }
            	// 추가적인 우회전 좌회전판단
                else {
                    int diff = (int)DisLeft - (int)DisRight;
                    if (diff > 6)       Car_Move(CAR_LEFT, SPD_80);
                    else if (diff < -6) Car_Move(CAR_RIGHT, SPD_80);
                    else                 Car_Move(CAR_LEFT, SPD_80);
                }
                auto_tick = current_Tick;
                auto_st = AUTO_STATE_AVOID;
            }
            // 일반 주행 및 벽 거리 멀어지게
            else {
                // 벽에 너무 가까우면 회피
                if (DisLeft < 15)       Car_Move(CAR_RIGHT, SPD_65);
                else if (DisRight < 15) Car_Move(CAR_LEFT, SPD_65);

                // 2. [사용자님 아이디어] 한쪽이 60 이상 넓게 뚫려 있다면 미리 몸을 틀어 코너 대비
                else if (DisLeft >= 70 && DisRight < 70) Car_Move(CAR_LEFT, SPD_65);
                else if (DisRight >= 70 && DisLeft < 70) Car_Move(CAR_RIGHT, SPD_65);

                // 3. 양쪽 다 좁거나, 양쪽 다 60 이상으로 뻥 뚫려 있으면 직진
                else                    Car_Move(CAR_FRONT, SPD_65);
            }
            break;

        case AUTO_STATE_AVOID:

            if (front_min > (Block_Distance_Front) || (current_Tick - auto_tick >= 40)) {
                auto_st = AUTO_STATE_SCAN;
            }
            break;

        case AUTO_STATE_BACK:
            if (current_Tick - auto_tick >= 250) {
                Car_Stop();
                auto_st = AUTO_STATE_SCAN;
            }
            break;

        default:
            auto_st = AUTO_STATE_SCAN;
            break;
    }
}


void DC_CONTROL_AUTO_PERCENT() {
    Ultrasonic_TriggerAll();
    uint32_t current_Tick = HAL_GetTick();

    uint8_t L = Ultrasonic_GetDistanceCm(US_LEFT);
    uint8_t C = Ultrasonic_GetDistanceCm(US_CENTER);
    uint8_t R = Ultrasonic_GetDistanceCm(US_RIGHT);

    // 100 이상의 거리는 100으로 통일
    uint8_t DisLeft   = (L == 0 || L > 100) ? 100 : L;
    uint8_t DisCenter = (C == 0 || C > 100) ? 100 : C;
    uint8_t DisRight  = (R == 0 || R > 100) ? 100 : R;

    //3개 반사값 비교해서 우선판단권 넘김
    uint8_t front_min = DisCenter;
    if (DisLeft < front_min)  front_min = DisLeft;
    if (DisRight < front_min) front_min = DisRight;

    switch (auto_st) {
        case AUTO_STATE_SCAN:
            // 최우선 순위 정면 충돌 직전 시 500ms 후진
            if (DisCenter < Crash_Distance) {
                Car_MovePercent(CAR_BACK, current_speed);
                actual_speed = current_speed;
                auto_tick = current_Tick;
                auto_st = AUTO_STATE_BACK;
            }
            // 본격적인 회피
            else if (front_min < Block_Distance_Front) {
                // 이중 비교 및 양 옆센서 값이 너무 낮을때 회피
            	if (DisLeft < (Block_Distance_Side)) {
                    Car_MovePercent(CAR_RIGHT, current_speed);
                    actual_speed = current_speed;
                }
                else if (DisRight < (Block_Distance_Side)) {
                    Car_MovePercent(CAR_LEFT, current_speed);
                    actual_speed = current_speed;
                }
            	// 추가적인 우회전 좌회전판단
                else {
                    int diff = (int)DisLeft - (int)DisRight;
                    if (diff > 6)       {Car_MovePercent(CAR_LEFT, current_speed); actual_speed = current_speed;}
                    else if (diff < -6) {Car_MovePercent(CAR_RIGHT, current_speed); actual_speed = current_speed;}
                    else                {Car_MovePercent(CAR_LEFT, current_speed); actual_speed = current_speed;}
                }
                auto_tick = current_Tick;
                auto_st = AUTO_STATE_AVOID;
            }
            // 일반 주행 및 벽 거리 멀어지게
            else {
                // 벽에 너무 가까우면 회피
                if (DisLeft < 15)       {Car_MovePercent(CAR_RIGHT, current_speed); actual_speed = current_speed;}
                else if (DisRight < 15) {Car_MovePercent(CAR_LEFT, current_speed); actual_speed = current_speed;}

                // 2. [사용자님 아이디어] 한쪽이 60 이상 넓게 뚫려 있다면 미리 몸을 틀어 코너 대비
                else if (DisLeft >= 70 && DisRight < 70) {Car_MovePercent(CAR_LEFT, current_speed); actual_speed = current_speed;}
                else if (DisRight >= 70 && DisLeft < 70) {Car_MovePercent(CAR_RIGHT, current_speed); actual_speed = current_speed;}

                // 3. 양쪽 다 좁거나, 양쪽 다 60 이상으로 뻥 뚫려 있으면 직진
                else                    {Car_MovePercent(CAR_FRONT, current_speed); actual_speed = current_speed;}
            }
            break;

        case AUTO_STATE_AVOID:

            if (front_min > (Block_Distance_Front) || (current_Tick - auto_tick >= 40)) {
                auto_st = AUTO_STATE_SCAN;
            }
            break;

        case AUTO_STATE_BACK:
            if (current_Tick - auto_tick >= 250) {
                Car_Stop();
                auto_st = AUTO_STATE_SCAN;
            }
            break;

        default:
            auto_st = AUTO_STATE_SCAN;
            break;
    }
}






/* ================= MODE FLAG ================= */

static bool st_auto = 0;
static bool st_manual = 1;

//*** AUTO MANUAL 판단 ***//
void ST_FLAG(uint8_t cmd)
{
	if(cmd == 'A')
	{
		st_auto = 1;
		st_manual = 0;
		auto_st = AUTO_STATE_SCAN;

		actual_speed = 0;
	}
	if(cmd == 'P')
	{
		Car_Stop();
		st_auto = 0;
		st_manual = 1;
		auto_st = AUTO_STATE_SCAN;

		actual_speed = 0;

	}
}


/* ================= MAIN STATE MACHINE ================= */


static uint8_t GetDangerCount(void)
{
    uint8_t danger_count = 0;

    if (temp_state == TEMP_STATE_DANGER) danger_count++;
    if (gas_state == GAS_DANGER) danger_count++;
    if (bms_state == BMS_STATE_DANGER) danger_count++;

    return danger_count;
}

static void UpdateCurrentSpeed(void)
{
    static uint32_t speed_time = 0;
    uint8_t danger_count;
    uint8_t reduction_step = 0;

    danger_count = GetDangerCount();
    last_danger_count = danger_count;

    if (danger_count == 1)      reduction_step = 1;
    else if (danger_count == 2) reduction_step = 3;
    else if (danger_count >= 3) reduction_step = 5;

    last_reduction_step = reduction_step;

    /* 정지 상태면 감속하지 않음 */
    if (actual_speed == 0)
    {
        speed_time = HAL_GetTick();
        return;
    }

    if (danger_count > 0)
    {
        if (HAL_GetTick() - speed_time >= 1000)   // 감속 주기
        {
            speed_time = HAL_GetTick();

            if (current_speed > 30 + reduction_step)
                current_speed -= reduction_step;
            else
                current_speed = 30;
        }
    }
    else
    {
        speed_time = HAL_GetTick();
    }
}

static void TRACE_TASK(void)
{
    static uint32_t trace_sensor_time = 0;
    static uint32_t trace_servo_time = 0;
    uint32_t now = HAL_GetTick();

    if (actual_speed != 0) return;

    if ((now - trace_sensor_time) >= 50)
    {
        trace_sensor_time = now;
        Sensor_Filter();
        Sun_Position();
        Target_Update();
    }

    if ((now - trace_servo_time) >= 20)
    {
        trace_servo_time = now;
        Servo_Move();
    }
}

void ST_MACHINE() {

	/* temp는 stop / manual / auto 전부 공통 감시 */
	TEMP_TASK();
	GAS_TASK();
	INA219_BMS_TASK();

	/* danger 상태에 따른 속도 감속 */
	UpdateCurrentSpeed();

	// 새 UART 명령 수신 시
	if (rxFlag)
	{
			HAL_UART_Transmit(&huart1, (uint8_t*)&rxCmd, 1, 10);
			ST_FLAG(rxCmd);
			rxFlag = 0;
	}

	// MANUAL 모드: 마지막 주행 명령을 계속 유지하며 현재 속도로 재적용
	if (st_manual == 1)
	{
			if (rxCmd != 'A' && rxCmd != 'P')
			{
					DC_CONTROL_MANUAL_PERCENT(rxCmd);
			}
	}

	if (st_auto == 1)
	{
		DC_CONTROL_AUTO_PERCENT();
	}

	TRACE_TASK();

}
