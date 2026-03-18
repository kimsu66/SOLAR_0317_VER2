#include "ina219_bms.h"

#define INA219_ADDR          (0x40 << 1)

#define INA219_REG_CONFIG    0x00
#define INA219_REG_SHUNTVOLT 0x01
#define INA219_REG_BUSVOLT   0x02
#define INA219_REG_POWER     0x03
#define INA219_REG_CURRENT   0x04
#define INA219_REG_CALIB     0x05

static INA219_BMS_t s_bms = {
    .hi2c = &hi2c1,
    .addr = INA219_ADDR,
    .current_lsb_uA = 100
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

int32_t INA219_BMS_ReadVoltage_mV(void)
{
    uint16_t raw;
    if (INA219_BMS_ReadReg(INA219_REG_BUSVOLT, &raw) != HAL_OK) return -1;

    raw >>= 3;
    return (int32_t)raw * 4;
}

int32_t INA219_BMS_ReadCurrent_mA(void)
{
    uint16_t raw;
    if (INA219_BMS_ReadReg(INA219_REG_CURRENT, &raw) != HAL_OK) return -1;

    return ((int16_t)raw * s_bms.current_lsb_uA) / 1000;
}

BMS_STATE INA219_BMS_GetState(int32_t voltage_mV, int32_t current_mA)
{
    if (current_mA >= 900 || voltage_mV <= 6400) return BMS_STATE_DANGER;
    if (current_mA >= 800 || voltage_mV <= 6800) return BMS_STATE_WARNING;
    return BMS_STATE_SAFE;
}

BMS_STATE INA219_BMS_Task(void)
{
    int32_t v = INA219_BMS_ReadVoltage_mV();
    int32_t i = INA219_BMS_ReadCurrent_mA();
    return INA219_BMS_GetState(v, i);
}
