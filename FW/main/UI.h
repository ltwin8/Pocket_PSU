#pragma once
#include <Arduino.h>

// Enum for logical UI pages
enum UIPage {
    PAGE_CHARGE,
    PAGE_V_OTG
};

const uint8_t char_Xposition[] = {2, 10, 18, 24, 36, 42, 52, 58}; 

class Display;  // forward declaration

class UI {
public:
    // Constructor
    UI(Display& disp);

    UIPage currentPage;

    UIPage getCurrentPage() const { return currentPage; }

    // Update the active page based on charge status
    void updatePage(bool isCharging);

    bool pageChanged() const { return currentPage != lastPage; }

    // Draw static elements (labels, borders) — only once per page load
    void renderStatic();

    void renderStaticIfChanged();

    // Draw dynamic values (voltages, currents) — call repeatedly
    void renderDynamic(uint16_t V_BAT = 0, uint16_t I_BAT = 0,
                       uint16_t I_BAT_SET = 0, uint16_t V_BUS = 0,
                       uint16_t V_OTG = 0, uint16_t I_OTG = 0,
                       uint16_t V_OTG_SET = 0, uint16_t I_OTG_SET = 0);

private:
    Display& display;
    UIPage lastPage;

    // Static and dynamic drawing functions for Charge page
    void drawChargeStatic();
    void drawChargeDynamic(uint16_t V_BAT, uint16_t I_BAT, uint16_t I_BAT_SET, uint16_t V_BUS);

    // Static and dynamic drawing functions for V_OTG page
    void draw_V_OTG_Static();
    void draw_V_OTG_Dynamic(uint16_t V_OTG, uint16_t I_OTG, uint16_t V_OTG_SET, uint16_t I_OTG_SET);

    
};

