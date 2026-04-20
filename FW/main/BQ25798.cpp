#include "BQ25798.h"
#include "HW.h"

// ------------------------
// Low-level register access
// ------------------------
void BQ25798::writeReg8(uint8_t reg, uint8_t val) {
  writeBQReg8(reg, val);
}

void BQ25798::writeReg16(uint8_t reg, uint16_t val) {

  writeBQReg16(reg, val);
}

uint8_t BQ25798::readReg8(uint8_t reg) {
  return readBQReg8(reg);
}

uint16_t BQ25798::readReg16(uint8_t reg) {
  return readBQReg16(reg);
}

uint16_t BQ25798::readReg16_le(uint8_t reg) {
  return readBQReg16_le(reg);
}

// ------------------------
// High-level API
// ------------------------
void BQ25798::begin() {

  setupRegisters();
  setChargeVoltage(12600);                  // 12.6V for 3S Li-ion
  setChargeCurrent(50);                     // 50mA min
  setOTG_Voltage(0);                        // 0V
  setOTG_Current(0);                        // 0mA
  setInputCurrentLimit(inputCurrentLimit);  // 200mA
}

bool BQ25798::isCharging() {
  uint8_t reg = readBQReg8(REG1C_CHARGER_STATUS_1);
  uint8_t bits = (reg >> 5) & 0b111;
  return (bits != 0);
}

/*
void BQ25798::toggleVOTG() {
    const uint8_t EN_OTG = 0b01000000; // Bit 6
    const uint8_t EN_ADC = 0b10000000; // Bit 6
    uint8_t reg = readBQReg8(REG_CHARGER_CONTROL_3);
    reg ^= EN_OTG; // toggle just bit 6
    writeReg8(REG_CHARGER_CONTROL_3, reg);
    uint8_t reg1 = readBQReg8(REG2E_ADC_Control);
    reg1 ^= EN_ADC; // toggle just bit 7
    writeReg8(REG2E_ADC_Control, reg1);
    
}
*/

void BQ25798::toggleVOTG() {
  const uint8_t EN_OTG = 0b01000000;  // Bit 6
  uint8_t reg = readBQReg8(REG_CHARGER_CONTROL_3);
  reg ^= EN_OTG;  // toggle just bit 6
  writeReg8(REG_CHARGER_CONTROL_3, reg);
}



void BQ25798::VOTG_ON() {
  const uint8_t OTG_ON = 0b01000000;  // Bit 6
  uint8_t reg = readBQReg8(REG_CHARGER_CONTROL_3);
  reg = OTG_ON;
  writeReg8(REG_CHARGER_CONTROL_3, reg);
}

void BQ25798::VOTG_OFF() {
  const uint8_t OTG_OFF = 0b00000000;  // Bit 6
  uint8_t reg = readBQReg8(REG_CHARGER_CONTROL_3);
  reg = OTG_OFF;
  writeReg8(REG_CHARGER_CONTROL_3, reg);
}

void BQ25798::up() {
  uint8_t reg1 = readBQReg8(REG2E_ADC_Control);
  reg1 = reg1 | 0b10000000;
  writeReg8(REG2E_ADC_Control, reg1);
}

void BQ25798::down() {
  uint8_t reg1 = readBQReg8(REG2E_ADC_Control);
  reg1 = reg1 & 0b01111111;
  writeReg8(REG2E_ADC_Control, reg1);
}

/// User adjustable ///
// - - - - - - - - - - - - - - - - - - - - - - //
void BQ25798::setChargeCurrent(uint16_t mA) {
  if (mA < 50) mA = 50;
  if (mA > 5000) mA = 5000;
  uint16_t regVal = mA / 10;
  writeReg16(REG_CHARGE_CURRENT_LIMIT, regVal);
}

void BQ25798::setOTG_Voltage(uint16_t mV) {
  const uint16_t VOTG_MIN = 2875;
  const uint16_t VOTG_MAX = 22000;
  if (mV < VOTG_MIN) mV = VOTG_MIN;
  if (mV > VOTG_MAX) mV = VOTG_MAX;
  uint16_t regVal = (mV - VOTG_MIN) / 10;
  writeReg16(REG_VOTG_REGULATION, regVal);
}

void BQ25798::setOTG_Current(uint16_t mA) {
  if (mA < 160) mA = 160;
  if (mA > 3360) mA = 3360;
  uint8_t regVal = mA / 40;
  writeReg8(REG_IOTG_REGULATION, regVal & 0x7F);
}
// - - - - - - - - - - - - - - - - - - - - - - //

uint16_t BQ25798::incChargeCurrent(uint16_t current_mA) {
  current_mA += 10;
  if (current_mA > chargeCurrentLimit) current_mA = chargeCurrentLimit;
  return current_mA;
}

uint16_t BQ25798::decChargeCurrent(uint16_t current_mA) {
  if (current_mA > 50) current_mA -= 10;
  return current_mA;
}

uint16_t BQ25798::incOTGVoltage(uint16_t voltage_mV, uint8_t step_mV) {
  if (voltage_mV < 2800) {
    voltage_mV = 2800;
  } else {
    voltage_mV += step_mV;  // fine control above 2800
    if (voltage_mV > 22000) voltage_mV = 22000;
  }

  return voltage_mV;
}

uint16_t BQ25798::decOTGVoltage(uint16_t voltage_mV, uint8_t step_mV) {
  if (voltage_mV > 2800) {
    if (voltage_mV > step_mV) voltage_mV -= step_mV;
    else voltage_mV = 2800;
  }

  return voltage_mV;
}



uint16_t BQ25798::incOTGCurrent(uint16_t current_mA) {
  current_mA += 40;
  if (current_mA > 3320) current_mA = 3320;
  return current_mA;
}

uint16_t BQ25798::decOTGCurrent(uint16_t current_mA) {
  if (current_mA > 160) current_mA -= 40;
  return current_mA;
}

// - - - - - - - - - - - - - - - - - - - - - - //

uint16_t BQ25798::readV_Bat() {
  writeReg8(REG2E_ADC_Control, 0b11000100);  //ADC OneShot Mode
  return readBQReg16(REG3B_VBAT_ADC);
}


int16_t BQ25798::readI_Bat() {
  writeReg8(REG2E_ADC_Control, 0b11000100);  //ADC OneShot Mode
  uint16_t current = readBQReg16_le(REG33_IBAT_ADC);
  return current;
}

uint16_t BQ25798::readV_Bus() {
  writeReg8(REG2E_ADC_Control, 0b11000100);  //ADC OneShot Mode
  return readBQReg16(REG35_VBUS_ADC);
}

int16_t BQ25798::readI_Bus() {
  writeReg8(REG2E_ADC_Control, 0b11000100);  //ADC OneShot Mode
  uint16_t current = readBQReg16_le(REG31_IBUS_ADC);
  return current;
}

uint8_t BQ25798::readChargerStatus() {
  writeReg8(REG2E_ADC_Control, 0b11000100);  //ADC OneShot Mode
  return readBQReg8(REG_CHARGER_STATUS_0);
}

void BQ25798::setChargeVoltage(uint16_t mV) {
  if (mV < 3000) mV = 3000;
  if (mV > 18800) mV = 18800;
  uint16_t regVal = mV / 10;
  writeReg16(REG_CHARGE_VOLTAGE_LIMIT, regVal);
}

void BQ25798::setInputCurrentLimit(uint16_t mA) {
  if (mA < 100) mA = 100;
  if (mA > 3300) mA = 3300;
  uint16_t regVal_iindpm = mA / 10;
  writeReg16(REG_INPUT_CURRENT_LIMIT, regVal_iindpm);
}

//Got bored writing function for all of these. They setup the chip so it charges and enables OTG.
void BQ25798::setupRegisters() {

  // Set Pre-charge control
  // bits 7-6 = vbat_low_v, bits 5-0 pre-charge current
  uint8_t reg0x8_val = (0b00000010);
  writeReg8(REG_PRECHARGE_CONTROL, reg0x8_val);

  // Set termination control
  uint8_t reg0x9_val = 0x00;  // 40mA
  writeReg8(REG_TERMINATION_CONTROL, reg0x9_val);

  // Set re-charge control
  uint8_t reg0xA_val = (0x2 << 6) | (0b11 << 4) | 0b1111;
  writeReg8(REG_RECHARGE_CONTROL, reg0xA_val);

  // Set charge control 0
  uint8_t reg0x0F_val = 0b10100000;
  writeReg8(REG_CHARGER_CONTROL_0, reg0x0F_val);

  // Set charge control 1
  uint8_t reg0x10_val = 0b10010000;
  writeReg8(REG_CHARGER_CONTROL_1, reg0x10_val);

  // Set charge control 2
  uint8_t reg0x11_val = 0b11000000;
  writeReg8(REG_CHARGER_CONTROL_2, reg0x11_val);

  // Set charge control 3
  uint8_t reg0x12_val = 0b00101000;
  writeReg8(REG_CHARGER_CONTROL_3, reg0x12_val);

  // Set charge control 4
  uint8_t reg0x13_val = 0b00001100;
  writeReg8(REG_CHARGER_CONTROL_4, reg0x13_val);

  // Set charge control 5
  uint8_t reg0x14_val = 0b00111111;
  writeReg8(REG_CHARGER_CONTROL_5, reg0x14_val);

  uint8_t reg0x16_val = 0b11000000;
  writeReg8(REG16_Temperature_Control, reg0x16_val);

  uint8_t reg0x17_val = 0b11111110;
  writeReg8(REG17_NTC_Control_0, reg0x17_val);

  uint8_t reg0x18_val = 0b01010101;
  writeReg8(REG18_NTC_Control_1, reg0x18_val);

  //uint8_t reg0x19_val = 0x09 //read only
  //writeReg8(REG19_ICO_Current_Limit, reg0x19_val);

  uint8_t reg0x2E_val = 0b11000100;  //ADC OneShot Mode
  writeReg8(REG2E_ADC_Control, reg0x2E_val);

  uint8_t reg0x47_val = 0b00000000;
  writeReg8(REG47_DPDM_Drive, reg0x47_val);

  uint8_t reg0x24_val = 0b00111001;
  writeReg8(REG24_TIMER_CONTROL, reg0x24_val);
}
#include "BQ25798.h"
#include "HW.h" 

// ------------------------
// Low-level register access
// ------------------------
void BQ25798::writeReg8(uint8_t reg, uint8_t val) {
    writeBQReg8(reg, val);
}

void BQ25798::writeReg16(uint8_t reg, uint16_t val) {

    writeBQReg16(reg, val);
}

uint8_t BQ25798::readReg8(uint8_t reg) {
    return readBQReg8(reg);
}

uint16_t BQ25798::readReg16(uint8_t reg) {
    return readBQReg16(reg);
}

uint16_t BQ25798::readReg16_le(uint8_t reg) {
    return readBQReg16_le(reg);
}

// ------------------------
// High-level API
// ------------------------
void BQ25798::begin() {
    
    setupRegisters();
    setChargeVoltage(12600);  // 12.6V for 3S Li-ion
    setChargeCurrent(50);     // 50mA min
    setOTG_Voltage(0);        // 0V
    setOTG_Current(0);        // 0mA
    setInputCurrentLimit(inputCurrentLimit);// 200mA
    
}

bool BQ25798::isCharging() {
    uint8_t reg = readBQReg8(REG1C_CHARGER_STATUS_1);
    uint8_t bits = (reg >> 5) & 0b111;
    return (bits != 0 );

}

void BQ25798::toggleVOTG() {
    const uint8_t EN_OTG = 0b01000000; // Bit 6
    uint8_t reg = readBQReg8(REG_CHARGER_CONTROL_3);
    reg ^= EN_OTG; // toggle just bit 6
    writeReg8(REG_CHARGER_CONTROL_3, reg);
    
}

void BQ25798::VOTG_ON() {
    const uint8_t OTG_ON = 0b01000000; // Bit 6
    uint8_t reg = readBQReg8(REG_CHARGER_CONTROL_3);
    reg = OTG_ON;
    writeReg8(REG_CHARGER_CONTROL_3, reg);
    
}

void BQ25798::VOTG_OFF() {
    const uint8_t OTG_OFF = 0b00000000; // Bit 6
    uint8_t reg = readBQReg8(REG_CHARGER_CONTROL_3);
    reg = OTG_OFF;
    writeReg8(REG_CHARGER_CONTROL_3, reg);
    
}

            /// User adjustable ///
// - - - - - - - - - - - - - - - - - - - - - - //
void BQ25798::setChargeCurrent(uint16_t mA) {
    if (mA < 50) mA = 50;
    if (mA > 5000) mA = 5000;
    uint16_t regVal = mA / 10;
    writeReg16(REG_CHARGE_CURRENT_LIMIT, regVal);
}

void BQ25798::setOTG_Voltage(uint16_t mV) {
    const uint16_t VOTG_MIN = 2875;
    const uint16_t VOTG_MAX = 22000;
    if (mV < VOTG_MIN) mV = VOTG_MIN;
    if (mV > VOTG_MAX) mV = VOTG_MAX;
    uint16_t regVal = (mV - VOTG_MIN) / 10;
    writeReg16(REG_VOTG_REGULATION, regVal);
}

void BQ25798::setOTG_Current(uint16_t mA) {
    if (mA < 160)  mA = 160;
    if (mA > 3360) mA = 3360;
    uint8_t regVal = mA / 40;
    writeReg8(REG_IOTG_REGULATION, regVal & 0x7F);
}
// - - - - - - - - - - - - - - - - - - - - - - //

uint16_t BQ25798::incChargeCurrent(uint16_t current_mA) {
    current_mA += 10;
    if (current_mA > chargeCurrentLimit) current_mA = chargeCurrentLimit;
    return current_mA;
}

uint16_t BQ25798::decChargeCurrent(uint16_t current_mA) {
    if (current_mA > 50) current_mA -= 10;
    return current_mA;
}

uint16_t BQ25798::incOTGVoltage(uint16_t voltage_mV, uint8_t step_mV) {
    if (voltage_mV < 1400) {
        voltage_mV = 1400;
    } else if (voltage_mV < 2800) {
        voltage_mV = 2800;
    } else {
        voltage_mV += step_mV;               // fine control above 2800
        if (voltage_mV > 22000) voltage_mV = 22000;
    }

    return voltage_mV;
}

uint16_t BQ25798::decOTGVoltage(uint16_t voltage_mV, uint8_t step_mV) {
    if (voltage_mV > 2800) {
        if (voltage_mV > step_mV) voltage_mV -= step_mV;
        else voltage_mV = 2800;
    } else if (voltage_mV > 1400) {
        voltage_mV = 1400;
    } else {
        voltage_mV = 0;
    }

    return voltage_mV;
}



uint16_t BQ25798::incOTGCurrent(uint16_t current_mA) {
    current_mA += 40;
    if (current_mA > 3320) current_mA = 3320;
    return current_mA;
}

uint16_t BQ25798::decOTGCurrent(uint16_t current_mA) {
    if (current_mA > 160) current_mA -= 40;
    return current_mA;
}

// - - - - - - - - - - - - - - - - - - - - - - //

uint16_t BQ25798::readV_Bat() {
    return readBQReg16(REG3B_VBAT_ADC);
}


int16_t BQ25798::readI_Bat() {
    uint16_t current = readBQReg16_le(REG33_IBAT_ADC);
    return current;
}

uint16_t BQ25798::readV_Bus() {
    return readBQReg16(REG35_VBUS_ADC);
}

int16_t BQ25798::readI_Bus() {
    uint16_t current = readBQReg16_le(REG31_IBUS_ADC);
    return current;
}

uint8_t BQ25798::readChargerStatus() {
    return readBQReg8(REG_CHARGER_STATUS_0);
}

void BQ25798::setChargeVoltage(uint16_t mV) {
    if (mV < 3000) mV = 3000;
    if (mV > 18800) mV = 18800;
    uint16_t regVal = mV / 10;
    writeReg16(REG_CHARGE_VOLTAGE_LIMIT, regVal);
}

void BQ25798::setInputCurrentLimit(uint16_t mA){
    if (mA < 100) mA = 100;
    if (mA > 3300) mA = 3300;
    uint16_t regVal_iindpm = mA / 10;
    writeReg16(REG_INPUT_CURRENT_LIMIT, regVal_iindpm);
}

//Got bored writing function for all of these. They setup the chip so it charges and enables OTG.
void BQ25798::setupRegisters(){

  // Set Pre-charge control 
  // bits 7-6 = vbat_low_v, bits 5-0 pre-charge current
  uint8_t reg0x8_val = (0b00000010);
  writeReg8(REG_PRECHARGE_CONTROL, reg0x8_val);

  // Set termination control 
  uint8_t reg0x9_val = 0x00; // 40mA 
  writeReg8(REG_TERMINATION_CONTROL, reg0x9_val);

    // Set re-charge control 
  uint8_t reg0xA_val = (0x2 << 6) | (0b11 << 4)  | 0b1111;
  writeReg8(REG_RECHARGE_CONTROL, reg0xA_val);

  // Set charge control 0
  uint8_t reg0x0F_val = 0b10100000;
  writeReg8(REG_CHARGER_CONTROL_0, reg0x0F_val);

  // Set charge control 1
  uint8_t reg0x10_val = 0b10010000;
  writeReg8(REG_CHARGER_CONTROL_1, reg0x10_val);

  // Set charge control 2
  uint8_t reg0x11_val = 0b11000000;
  writeReg8(REG_CHARGER_CONTROL_2, reg0x11_val);

  // Set charge control 3
  uint8_t reg0x12_val = 0b00101000;
  writeReg8(REG_CHARGER_CONTROL_3, reg0x12_val);

  // Set charge control 4
  uint8_t reg0x13_val = 0b00001100;
  writeReg8(REG_CHARGER_CONTROL_4, reg0x13_val);

  // Set charge control 5
  uint8_t reg0x14_val = 0b00111111;
  writeReg8(REG_CHARGER_CONTROL_5, reg0x14_val);

  uint8_t reg0x16_val = 0b11000000;
  writeReg8(REG16_Temperature_Control, reg0x16_val);

  uint8_t reg0x17_val = 0b11111110;
  writeReg8(REG17_NTC_Control_0, reg0x17_val); 

  uint8_t reg0x18_val = 0b01010101;
  writeReg8(REG18_NTC_Control_1, reg0x18_val);

  //uint8_t reg0x19_val = 0x09 //read only
  //writeReg8(REG19_ICO_Current_Limit, reg0x19_val);

  uint8_t reg0x2E_val = 0b10000100;
  writeReg8(REG2E_ADC_Control, reg0x2E_val);

  uint8_t reg0x47_val = 0b00000000;
  writeReg8(REG47_DPDM_Drive, reg0x47_val);

  uint8_t reg0x24_val = 0b00111001;
  writeReg8(REG24_TIMER_CONTROL, reg0x24_val);
  

}


