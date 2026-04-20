#include "Display.h"
#include "HW.h"
#include "BQ25798.h"

void Display::sendCommand(uint8_t cmd) {
    writeOLEDCommand(cmd);
}

void Display::sendData(uint8_t data) {
    writeOLEDData(data);
}

void Display::begin() {
    TinyMegaI2C.init();

    // Basic initialization sequence for SSD1306
    sendCommand(0xAE); // Display OFF
    sendCommand(0x20); // Set Memory Addressing Mode
    sendCommand(0x00); // Horizontal addressing
    sendCommand(0xB0); // Set page 0
    sendCommand(0xC0); // COM Output Scan Direction
    sendCommand(0x00); // Low column start address
    sendCommand(0x10); // High column start address
    sendCommand(0x40); // Start line address
    sendCommand(0x81); // Contrast control
    sendCommand(0xFF); // Max contrast
    sendCommand(0xA0); // Segment remap
    sendCommand(0xA6); // Normal display
    sendCommand(0xA8); // Multiplex ratio
    sendCommand(0x3F);
    sendCommand(0xD3); // Display offset
    sendCommand(0x00);
    sendCommand(0xD5); // Display clock divide
    sendCommand(0xF0);
    sendCommand(0xD9); // Pre-charge
    sendCommand(0x22);
    sendCommand(0xDA); // COM pins
    sendCommand(0x12);
    sendCommand(0xDB); // VCOMH
    sendCommand(0x20);
    sendCommand(0x8D); // Charge pump
    sendCommand(0x14);
    sendCommand(0xD3); // Set display offset
    sendCommand(0x20); // Move visible down
    sendCommand(0xAF); // Display ON

    
}

void Display::clearScreen() {
    for (uint8_t page = 4; page < 8; page++) {
    sendCommand(0xB0 | page);
        sendCommand(0x00);
        sendCommand(0x10 | SSD1306_X_origin >> 4);
        TinyMegaI2C.start(ADDRESS, 0);
        TinyMegaI2C.write(0x40);
        for (uint8_t col = 0; col < 64; col++) {
            TinyMegaI2C.write(0x00);
        }
        TinyMegaI2C.stop();
    }

    sendCommand(0xB0 | PAGE_0);
    sendCommand(0x00);
    sendCommand(0x10 | SSD1306_X_origin >> 4);
}

int Display::getCharacterIndex(char c) {
    for (uint8_t i = 0; i < strlen(characterMap) - 1; i++) {
        if (characterMap[i] == c)
            return i;
    }
    return -1;
}

void Display::drawChar(char c, uint8_t page, uint8_t col) {
    int index = getCharacterIndex(c);
    if (index < 0) return;

    sendCommand(0xB0 | page);
    sendCommand(0x00 | (col & 0x0F));
    sendCommand(0x10 | ((col >> 4) & 0x0F));

    TinyMegaI2C.start(ADDRESS, 0);
    TinyMegaI2C.write(0x40);

    for (uint8_t i = 0; i < CHAR_WIDTH; i++) {
        uint8_t b = pgm_read_byte(&(ssd1306xled_font6x8[index * CHAR_WIDTH + i]));
        TinyMegaI2C.write(b);
    }

    TinyMegaI2C.stop();
}

void Display::drawCurrent(uint16_t current_mA, uint8_t page, uint8_t x_origin) {
    uint8_t x = x_origin;
    uint16_t thousands = 1000; 

    bool leadingDigitPrinted = false;

    for (uint8_t i = 0; i < 4; i++) { // 4 digits
        uint8_t digit = (current_mA / thousands) % 10;

        if (digit != 0 || thousands == 1 || leadingDigitPrinted) {
            drawChar(characterMap[digit], page, x);
            leadingDigitPrinted = true;
        } else {
            drawChar(' ', page, x); // leading space
        }

        x += CHAR_WIDTH;
        thousands /= 10;
      
    }

    drawChar('m', page, x);
    drawChar('A', page, x + CHAR_WIDTH);

}

void Display::drawVoltage(uint16_t voltage_mV, uint8_t page, uint8_t x_origin) {

    uint8_t x = x_origin;

    uint8_t intPart = voltage_mV / 1000;    
    uint16_t fracPart = voltage_mV % 1000;

    // Leading space if single-digit integer
    if (intPart < 10) {
        drawChar(' ', page, x); x += CHAR_WIDTH;
    } else {
        drawChar(characterMap[intPart / 10], page, x); x += CHAR_WIDTH;
    }

    // Ones digit 
    drawChar(characterMap[intPart % 10], page, x); x += CHAR_WIDTH;

    // Decimal point
    drawChar('.', page, x); x += CHAR_WIDTH;

    // Fractional digits (hundredths, tenths, thousandths)
    drawChar(characterMap[fracPart / 100], page, x); x += CHAR_WIDTH;
    drawChar(characterMap[(fracPart / 10) % 10], page, x); x += CHAR_WIDTH;
    drawChar(characterMap[fracPart % 10], page, x); x += CHAR_WIDTH;

    // Unit
    drawChar('V', page, x);

}

void Display::drawColumn(uint8_t page, uint8_t col) {
    // Set page
    sendCommand(0xB0 | page);

    // Set column (split into lower + upper nibble)
    sendCommand(0x00 | (col & 0x0F));       
    sendCommand(0x10 | ((col >> 4) & 0x0F));

    // Send data
    TinyMegaI2C.start(ADDRESS, 0);
    TinyMegaI2C.write(0x40); 
    TinyMegaI2C.write(0xFF);
    TinyMegaI2C.stop();

}

void Display::clearColumn(uint8_t page, uint8_t col) {
    // Set page
    sendCommand(0xB0 | page);

    // Set column (split into lower + upper nibble)
    sendCommand(0x00 | (col & 0x0F));       
    sendCommand(0x10 | ((col >> 4) & 0x0F));

    // Send data
    TinyMegaI2C.start(ADDRESS, 0);
    TinyMegaI2C.write(0x40); 
    TinyMegaI2C.write(0x00);
    TinyMegaI2C.stop();

}

void Display::drawBorder() {

    uint8_t pages[] = { PAGE_0, PAGE_1, PAGE_2, PAGE_3 };
    uint8_t leftCol  = SSD1306_X_origin;

    for (uint8_t i = 0; i < sizeof(pages)/sizeof(pages[0]); i++) { //sizeof = bytes
        drawColumn(pages[i], leftCol);
    }
}

void Display::clearBorder() {

    uint8_t pages[] = { PAGE_0, PAGE_1, PAGE_2, PAGE_3 };
    uint8_t leftCol  = SSD1306_X_origin;

    for (uint8_t i = 0; i < sizeof(pages)/sizeof(pages[0]); i++) { //sizeof = bytes
        clearColumn(pages[i], leftCol);
    }
}

void Display::toggleBorder() {
    if (borderVisible) {
        clearBorder();
        //bq.down();
    } else {
        drawBorder();
        //bq.up();
    }
    borderVisible = !borderVisible;
}

void Display::nextPage(UIPage currentPage) {
    if (currentPage == PAGE_CHARGE) {
        // Only PAGE_2 is adjustable on charge page
        activePage = PAGE_2;
    } 
    else if (currentPage == PAGE_V_OTG) {
        // Only PAGE_2 and PAGE_3 are adjustable
        if (activePage == PAGE_2) activePage = PAGE_3;
        else activePage = PAGE_2;
    }
}

void Display::prevPage(UIPage currentPage) {
    nextPage(currentPage);
}

void Display::setActivePage(DisplayPage page) {
    activePage = page;
}

DisplayPage Display::getActivePage() const {
    return activePage;
}

void Display::indicateActivePage() {
    uint8_t rightCol = SSD1306_X_origin + SSD1306_X_pixels - 1;

    drawColumn(activePage, rightCol);

}




