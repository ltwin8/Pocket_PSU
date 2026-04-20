#include "UI.h"
#include "Display.h"

UI::UI(Display& disp) : display(disp), currentPage(PAGE_CHARGE) {}

// static parts
void UI::renderStatic() {
    switch(currentPage) {
        case PAGE_CHARGE: drawChargeStatic(); break;
        case PAGE_V_OTG:  draw_V_OTG_Static();    break;
    }
}

// dynamic parts
void UI::renderDynamic(uint16_t V_BAT, uint16_t I_BAT, uint16_t I_BAT_SET, uint16_t V_BUS,
                       uint16_t V_OTG, uint16_t I_OTG, uint16_t V_OTG_SET, uint16_t I_OTG_SET) {
    switch(currentPage) {
        case PAGE_CHARGE:
            drawChargeDynamic(V_BAT, I_BAT, I_BAT_SET, V_BUS);
            break;
        case PAGE_V_OTG:
            draw_V_OTG_Dynamic(V_OTG, I_OTG, V_OTG_SET, I_OTG_SET);
            break;
    }

}

void UI::renderStaticIfChanged() {
    if (currentPage != lastPage) {
        renderStatic();
        lastPage = currentPage;
    }
}

void UI::updatePage(bool charging) {
    currentPage = charging ? PAGE_V_OTG : PAGE_CHARGE;
}

// only draw once per page load
void UI::drawChargeStatic() {
    display.drawChar('V', PAGE_0, char_Xposition[4]);
    display.drawChar('b', PAGE_0, char_Xposition[5]);

    display.drawChar('I', PAGE_1, char_Xposition[4]);
    display.drawChar('b', PAGE_1, char_Xposition[5]);

    display.drawChar('I', PAGE_2, char_Xposition[4]);
    display.drawChar('s', PAGE_2, char_Xposition[5]);

    display.drawChar('V', PAGE_3, char_Xposition[4]);
    display.drawChar('i', PAGE_3, char_Xposition[5]);

}

void UI::drawChargeDynamic(uint16_t V_BAT, uint16_t I_BAT, uint16_t I_BAT_SET, uint16_t V_BUS) {
    display.drawVoltage(V_BAT,      PAGE_0, char_Xposition[6]);    
    display.drawCurrent(I_BAT,      PAGE_1, char_Xposition[7]);
    display.drawCurrent(I_BAT_SET,  PAGE_2, char_Xposition[7]);  
    display.drawVoltage(V_BUS,      PAGE_3, char_Xposition[6]);
}

// only draw once per page load
void UI::draw_V_OTG_Static() {

    display.drawChar('V', PAGE_0, char_Xposition[4]);
    display.drawChar(':', PAGE_0, char_Xposition[5]);

    display.drawChar('I', PAGE_1, char_Xposition[4]);
    display.drawChar(':', PAGE_1, char_Xposition[5]);

    display.drawChar('V', PAGE_2, char_Xposition[4]);
    display.drawChar('s', PAGE_2, char_Xposition[5]);

    display.drawChar('I', PAGE_3, char_Xposition[4]);
    display.drawChar('s', PAGE_3, char_Xposition[5]);

}

void UI::draw_V_OTG_Dynamic(uint16_t V_OTG, uint16_t I_OTG, uint16_t V_OTG_SET, uint16_t I_OTG_SET) {
    display.drawVoltage(V_OTG,      PAGE_0, char_Xposition[6]);    
    display.drawCurrent(I_OTG,      PAGE_1, char_Xposition[7]);
    display.drawVoltage(V_OTG_SET,  PAGE_2, char_Xposition[6]);  
    display.drawCurrent(I_OTG_SET,  PAGE_3, char_Xposition[7]);
}


