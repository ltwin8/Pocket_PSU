#pragma once
#include <stdint.h>

// Register defines (from your existing code)
#define REG_CHARGE_VOLTAGE_LIMIT    0x01
#define REG_CHARGE_CURRENT_LIMIT    0x03
#define REG_INPUT_CURRENT_LIMIT     0x06
#define REG_VOTG_REGULATION         0x0B
#define REG_IOTG_REGULATION         0x0D
#define REG_CHARGER_STATUS_0        0x1B
#define REG3B_VBAT_ADC              0x3B
#define REG33_IBAT_ADC              0x33
#define REG35_VBUS_ADC              0x35
#define REG31_IBUS_ADC              0x31
#define REG_PRECHARGE_CONTROL       0x08
#define REG_TERMINATION_CONTROL     0x09
#define REG_RECHARGE_CONTROL        0x0A
#define REG_CHARGER_CONTROL_0       0x0F
#define REG_CHARGER_CONTROL_1       0x10
#define REG_CHARGER_CONTROL_2       0x11
#define REG_CHARGER_CONTROL_3       0x12
#define REG_CHARGER_CONTROL_4       0x13
#define REG_CHARGER_CONTROL_5       0x14
#define REG16_Temperature_Control   0x16
#define REG17_NTC_Control_0         0x17
#define REG18_NTC_Control_1         0x18
#define REG2E_ADC_Control           0x2E
#define REG47_DPDM_Drive            0x47
#define REG1C_CHARGER_STATUS_1      0x1C
#define REG24_TIMER_CONTROL         0x24

#define chargeCurrentLimit          200 //mA
#define inputCurrentLimit           500  //mA

class BQ25798 {
public:
    BQ25798() = default;

    void begin();

    void setChargeVoltage(uint16_t mV);
    void setChargeCurrent(uint16_t mA);
    void setOTG_Voltage(uint16_t mV);
    void setOTG_Current(uint16_t mA);

    void setupRegisters();

    uint16_t readV_Bat();
    int16_t readI_Bat();
    uint16_t readV_Bus();
    int16_t readI_Bus();

    uint8_t readChargerStatus();
    bool isCharging();
    void toggleVOTG();
    void VOTG_ON();
    void VOTG_OFF();
    void up();
    void down();

    uint16_t incChargeCurrent(uint16_t current_mA);
    uint16_t decChargeCurrent(uint16_t current_mA);
    uint16_t incOTGVoltage(uint16_t voltage_mV, uint8_t step_mV);
    uint16_t decOTGVoltage(uint16_t voltage_mV, uint8_t step_mV);
    uint16_t incOTGCurrent(uint16_t current_mA);
    uint16_t decOTGCurrent(uint16_t current_mA);
    void setInputCurrentLimit(uint16_t mA);


private:
    void writeReg8(uint8_t reg, uint8_t val);
    void writeReg16(uint8_t reg, uint16_t val);
    uint8_t readReg8(uint8_t reg);
    uint16_t readReg16(uint8_t reg);
    uint16_t readReg16_le(uint8_t reg);
    
};
#pragma once
#include <stdint.h>

// Register defines (from your existing code)
#define REG_CHARGE_VOLTAGE_LIMIT    0x01
#define REG_CHARGE_CURRENT_LIMIT    0x03
#define REG_INPUT_CURRENT_LIMIT     0x06
#define REG_VOTG_REGULATION         0x0B
#define REG_IOTG_REGULATION         0x0D
#define REG_CHARGER_STATUS_0        0x1B
#define REG3B_VBAT_ADC              0x3B
#define REG33_IBAT_ADC              0x33
#define REG35_VBUS_ADC              0x35
#define REG31_IBUS_ADC              0x31
#define REG_PRECHARGE_CONTROL       0x08
#define REG_TERMINATION_CONTROL     0x09
#define REG_RECHARGE_CONTROL        0x0A
#define REG_CHARGER_CONTROL_0       0x0F
#define REG_CHARGER_CONTROL_1       0x10
#define REG_CHARGER_CONTROL_2       0x11
#define REG_CHARGER_CONTROL_3       0x12
#define REG_CHARGER_CONTROL_4       0x13
#define REG_CHARGER_CONTROL_5       0x14
#define REG16_Temperature_Control   0x16
#define REG17_NTC_Control_0         0x17
#define REG18_NTC_Control_1         0x18
#define REG2E_ADC_Control           0x2E
#define REG47_DPDM_Drive            0x47
#define REG1C_CHARGER_STATUS_1      0x1C
#define REG24_TIMER_CONTROL         0x24

#define chargeCurrentLimit          200 //mA
#define inputCurrentLimit           500  //mA

class BQ25798 {
public:
    BQ25798() = default;

    void begin();

    void setChargeVoltage(uint16_t mV);
    void setChargeCurrent(uint16_t mA);
    void setOTG_Voltage(uint16_t mV);
    void setOTG_Current(uint16_t mA);
    

    void setupRegisters();

    uint16_t readV_Bat();
    int16_t readI_Bat();
    uint16_t readV_Bus();
    int16_t readI_Bus();

    uint8_t readChargerStatus();
    bool isCharging();
    void toggleVOTG();
    void VOTG_ON();
    void VOTG_OFF();

    uint16_t incChargeCurrent(uint16_t current_mA);
    uint16_t decChargeCurrent(uint16_t current_mA);
    uint16_t incOTGVoltage(uint16_t voltage_mV, uint8_t step_mV);
    uint16_t decOTGVoltage(uint16_t voltage_mV, uint8_t step_mV);
    uint16_t incOTGCurrent(uint16_t current_mA);
    uint16_t decOTGCurrent(uint16_t current_mA);
    void setInputCurrentLimit(uint16_t mA);


private:
    void writeReg8(uint8_t reg, uint8_t val);
    void writeReg16(uint8_t reg, uint16_t val);
    uint8_t readReg8(uint8_t reg);
    uint16_t readReg16(uint8_t reg);
    uint16_t readReg16_le(uint8_t reg);
    
};
