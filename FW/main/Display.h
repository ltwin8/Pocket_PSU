#pragma once

#include <Arduino.h>
#include <TinyMegaI2C.h>
#include "font6x8_minimal.h"
#include "UI.h"  // include UI.h because we need UIPage
#include "BQ25798.h"
#include "HW.h" 

// Display page enum — used internally in Display
enum DisplayPage {  
    PAGE_0 = 4,
    PAGE_1 = 5,
    PAGE_2 = 6,
    PAGE_3 = 7,
    PAGE_NONE = 3 //off screen  
};


class Display {
public:
    void begin();
    void clearScreen();
    void drawChar(char c, uint8_t page, uint8_t col);
    void drawCurrent(uint16_t current_mA, uint8_t page, uint8_t x_origin);
    void drawVoltage(uint16_t voltage_mV, uint8_t page, uint8_t x_origin);
    void drawBorder();
    void clearBorder();
    void toggleBorder();
    uint8_t getCharXPosition(uint8_t index);
    
    void nextPage(UIPage currentPage);  // pass the logical page
    void prevPage(UIPage currentPage);

    void setActivePage(DisplayPage page);
    DisplayPage getActivePage() const;

    void indicateActivePage();

    void clearColumn(uint8_t page, uint8_t col);

    #define SSD1708_X_pixels 128
    #define SSD1708_Y_pixels 64

    #define SSD1306_X_pixels 64
    #define SSD1306_Y_pixels 32
    #define SSD1306_X_origin ((SSD1708_X_pixels - SSD1306_X_pixels) / 2)



private:


    DisplayPage lastFlashPage;
    
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
    void setupCharPositions();
    int getCharacterIndex(char c);
    void drawColumn(uint8_t page, uint8_t col);
    
    bool borderVisible = false;

    DisplayPage activePage = PAGE_NONE;

    static const uint8_t ADDRESS = 0x3C;
    static const uint8_t CHAR_WIDTH = 6;
    static const uint8_t NUM_CHARS = 9;
    static const uint8_t BORDER_PIXEL_OFFSET = 2;

    uint8_t char_Xposition[NUM_CHARS];
};
