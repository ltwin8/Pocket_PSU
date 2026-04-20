#include "HW.h"

// -------- BQ25798 --------

// 16-bit register write (MSB first)
void writeBQReg16(uint8_t reg, uint16_t val) {
    TinyMegaI2C.start(BQ25798_ADDR, 0);
    TinyMegaI2C.write(reg);
    TinyMegaI2C.write(val >> 8);
    TinyMegaI2C.write(val & 0xFF);
    TinyMegaI2C.stop();
}

// 8-bit register write
void writeBQReg8(uint8_t reg, uint8_t val) {
    TinyMegaI2C.start(BQ25798_ADDR, 0);
    TinyMegaI2C.write(reg);
    TinyMegaI2C.write(val);
    TinyMegaI2C.stop();
}

// 8-bit register read
uint8_t readBQReg8(uint8_t reg) {
    TinyMegaI2C.start(BQ25798_ADDR, 0);
    TinyMegaI2C.write(reg);
    TinyMegaI2C.stop();

    TinyMegaI2C.start(BQ25798_ADDR, 1);
    uint8_t val = TinyMegaI2C.read();
    TinyMegaI2C.stop();
    return val;
}

// 16-bit register read msb first
uint16_t readBQReg16(uint8_t reg) {
    TinyMegaI2C.start(BQ25798_ADDR, 0);
    TinyMegaI2C.write(reg);
    TinyMegaI2C.stop();

    TinyMegaI2C.start(BQ25798_ADDR, 2);
    uint8_t msb = TinyMegaI2C.read();
    uint8_t lsb = TinyMegaI2C.readLast();
    TinyMegaI2C.stop();

    return (msb << 8) | lsb;
}

// 16-bit register read lsb first
uint16_t readBQReg16_le(uint8_t reg) {
    TinyMegaI2C.start(BQ25798_ADDR, 0);
    TinyMegaI2C.write(reg);
    TinyMegaI2C.stop();

    TinyMegaI2C.start(BQ25798_ADDR, 2);
    uint8_t lsb = TinyMegaI2C.read();
    uint8_t msb = TinyMegaI2C.readLast();
    TinyMegaI2C.stop();

    return (lsb << 8) | msb;

}

// -------- OLED --------

// Send a command to the OLED
void writeOLEDCommand(uint8_t cmd) {
    TinyMegaI2C.start(OLED_ADDR, 0);
    TinyMegaI2C.write(0x00); // Command mode
    TinyMegaI2C.write(cmd);
    TinyMegaI2C.stop();
}

// Send a data byte to the OLED
void writeOLEDData(uint8_t data) {
    TinyMegaI2C.start(OLED_ADDR, 0);
    TinyMegaI2C.write(0x40); // Data mode
    TinyMegaI2C.write(data);
    TinyMegaI2C.stop();
}


