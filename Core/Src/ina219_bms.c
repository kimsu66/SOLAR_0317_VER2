#include "ina219_bms.h"

#define INA219_ADDR          (0x40 << 1)

#define INA219_REG_CONFIG    0x00
#define INA219_REG_SHUNTVOLT 0x01
#define INA219_REG_BUSVOLT   0x02
#define INA219_REG_POWER     0x03
#define INA219_REG_CURRENT   0x04
#define INA219_REG_CALIB     0x05

#define CURRENT_WARNING 		1500
#define CURRENT_DANGER	 		2500

#define VOLTAGE_UNDER_DANGER		8000
#define VOLTAGE_UNDER_WARNING		9000 // mV

#define VOLTAGE_OVER_WARNING		12000
#define VOLTAGE_OVER_DANGER			13200

static INA219_BMS_t s_bms = {
    .hi2c = &hi2c1,
    .addr = INA219_ADDR,
		.rshunt_mohm = 100   // 0.1Ω = 100mΩ
};

static HAL_StatusTypeDef INA219_BMS_WriteReg(uint8_t reg, uint16_t data)
{
    uint8_t buf[2];
    buf[0] = (data >> 8) & 0xFF;
    buf[1] = data & 0xFF;
    return HAL_I2C_Mem_Write(s_bms.hi2c, s_bms.addr, reg, I2C_MEMADD_SIZE_8BIT, buf, 2, 100);
}

static HAL_StatusTypeDef INA219_BMS_ReadReg(uint8_t reg, uint16_t *data)
{
    uint8_t buf[2];
    HAL_StatusTypeDef ret;

    ret = HAL_I2C_Mem_Read(s_bms.hi2c, s_bms.addr, reg, I2C_MEMADD_SIZE_8BIT, buf, 2, 100);
    if (ret != HAL_OK) return ret;

    *data = ((uint16_t)buf[0] << 8) | buf[1];
    return HAL_OK;
}

uint8_t INA219_BMS_Init(void)
{
    /* 예시 config/calibration 값 */
    if (INA219_BMS_WriteReg(INA219_REG_CONFIG, 0x399F) != HAL_OK) return 0;
    if (INA219_BMS_WriteReg(INA219_REG_CALIB, 4096) != HAL_OK) return 0;
    return 1;
}

int32_t INA219_BMS_ReadCurrent_mA(void)
{
    uint16_t raw;
    int32_t shunt_uV;

    if (INA219_BMS_ReadReg(INA219_REG_SHUNTVOLT, &raw) != HAL_OK) return -1;

    shunt_uV = (int32_t)((int16_t)raw) * 10;   // 10uV/bit, signed
    return shunt_uV / s_bms.rshunt_mohm;       // mA
}

int32_t INA219_BMS_ReadVoltage_mV(void)
{
		uint16_t raw_bus, raw_shunt;
		int32_t vbus_mV;
		int32_t shunt_uV;

		// 1. V- (BUS VOLTAGE)
		if (INA219_BMS_ReadReg(INA219_REG_BUSVOLT, &raw_bus) != HAL_OK) return -1;
		raw_bus >>= 3;
		vbus_mV = (int32_t)raw_bus * 4;   // mV

		// 2. Shunt Voltage (V+ - V-)
		if (INA219_BMS_ReadReg(INA219_REG_SHUNTVOLT, &raw_shunt) != HAL_OK) return -1;
		shunt_uV = (int32_t)((int16_t)raw_shunt) * 10;  // uV

		// 3. V+ 계산
		return vbus_mV + (shunt_uV / 1000);  // mV
}


CURRENT_STATE INA219_BMS_GetCurrentState(int32_t current_mA)
{
		if (current_mA >= CURRENT_DANGER) return CURRENT_STATE_DANGER;
    if (current_mA >= CURRENT_WARNING) return CURRENT_STATE_WARNING;
    return CURRENT_STATE_SAFE;
}

VOLTAGE_STATE INA219_BMS_GetVoltageState(int32_t voltage_mV)
{
		if (voltage_mV <= VOLTAGE_UNDER_DANGER) 	return VOLTAGE_STATE_UNDER_DANGER;
		if (voltage_mV <= VOLTAGE_UNDER_WARNING) 	return VOLTAGE_STATE_UNDER_WARNING;

    if (voltage_mV >= VOLTAGE_OVER_DANGER) 	return VOLTAGE_STATE_OVER_DANGER;
    if (voltage_mV >= VOLTAGE_OVER_WARNING)  return VOLTAGE_STATE_OVER_WARNING;
    return VOLTAGE_STATE_SAFE;
}
