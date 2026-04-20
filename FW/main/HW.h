#pragma once
#include <TinyMegaI2C.h>
#include <stdint.h>

// I2C device addresses
#define BQ25798_ADDR 0x6B
#define OLED_ADDR    0x3C
//#define STUSB4500_ADDR 0x28

// BQ25798 functions
void writeBQReg16(uint8_t reg, uint16_t val);
void writeBQReg8(uint8_t reg, uint8_t val);
uint8_t readBQReg8(uint8_t reg);
uint16_t readBQReg16(uint8_t reg);
uint16_t readBQReg16_le(uint8_t reg);

// OLED functions
void writeOLEDCommand(uint8_t cmd);
void writeOLEDData(uint8_t data);

